#pragma once

#include "IPCUtility.h"
#include "IPCShared.h"
#include "IPCChannel.h"



IPCChannel hml_to_launcher_channel("Local\\HML_TO_LAUNCHER_SHAREDMEM", "Local\\HML_TO_LAUNCHER_EVENT");
IPCChannel hml_to_overlay_channel("Local\\HML_TO_OVERLAY_SHAREDMEM", "Local\\HML_TO_OVERLAY_EVENT");

std::mutex inMutex;
bool has_incoming_data;
to_overlay_struct incoming_data;

std::mutex outMutex;
bool has_outgoing_data;
to_launcher_struct outgoing_data;

#pragma region  READ_WRITE_THREADS

std::atomic<bool> is_running{ false };
std::thread readThread;
std::thread writeThread;
HANDLE writeEvent = CreateEventA(NULL, FALSE, FALSE, NULL);

void WriteLoop()
{
    while (is_running)
    {
        WaitForSingleObject(writeEvent, INFINITE);

        to_launcher_struct msg;
        {
            std::lock_guard<std::mutex> lock(outMutex);
            if (!has_outgoing_data)
                continue;
            msg = outgoing_data;
            has_outgoing_data = false;
        }
        hml_to_launcher_channel.Write(&msg, sizeof(msg));
    }
}
void ReadLoop()
{
    to_overlay_struct msg{};
    while (is_running)
    {
        if (hml_to_overlay_channel.Read(&msg, sizeof(msg)))
        {
            //MessageBoxA(0, "[CLS-CLIENT] about to dequeue read", "debug", 0);
            {
                std::lock_guard<std::mutex> lock(inMutex);
                has_incoming_data = true;
                incoming_data = msg;
            }
        }
    }
}
#pragma endregion

void IPC_QueueWrite(const to_launcher_struct& msg)
{
    {
        std::lock_guard<std::mutex> lock(outMutex);
        has_outgoing_data = true;
        outgoing_data = msg;
    }
    SetEvent(writeEvent);
}

bool IPC_PollRead(to_overlay_struct& out)
{
    std::lock_guard<std::mutex> lock(inMutex);
    if (!has_incoming_data)
        return false;

    has_incoming_data = false;
    out = incoming_data;
    return true;
}

void OverlayIPC_Init()
{
    if (sizeof(to_overlay_struct) >= SHARED_BUFFER_SIZE)
    {
        MessageBoxA(0, "[OVERLAY-IPC] struct 'to_launcher_struct' is too big to fit into shared mem buffer", "OVERLAY IPC FAILURE", 0);
        return;
    }
    if (sizeof(to_launcher_struct) >= SHARED_BUFFER_SIZE)
    {
        MessageBoxA(0, "[OVERLAY-IPC] struct 'to_overlay_struct' is too big to fit into shared mem buffer", "OVERLAY IPC FAILURE", 0);
        return;
    }

    is_running = true;
    readThread = std::thread(&ReadLoop);
    writeThread = std::thread(&WriteLoop);

    // TODO: we need to wake the read thread? as its going to miss the first event call?
}
void OverlayIPC_Shutdown()
{
    bool expected = true;
    if (!is_running.compare_exchange_strong(expected, false))
        return;

    // wake write loop so it can exit
    SetEvent(writeEvent);

    if (readThread.joinable())  readThread.join();
    if (writeThread.joinable()) writeThread.join();
}