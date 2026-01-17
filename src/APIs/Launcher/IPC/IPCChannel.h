#pragma once


#pragma pack(push, 1)
struct SharedBuffer
{
public:
    int PayloadSize;
    uint32_t PayloadHash;
    uint8_t Payload[SHARED_BUFFER_SIZE];
};
#pragma pack(pop)

class IPCChannel
{
private:
    HANDLE hMapFile = nullptr;
    HANDLE hEvent = nullptr;
    SharedBuffer* buffer = nullptr;
public:
    IPCChannel(const char* memName, const char* eventName)
    {
        hMapFile = CreateFileMappingA(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            0,
            sizeof(SharedBuffer),
            memName
        );

        if (!hMapFile)
            throw std::runtime_error("CreateFileMapping failed");

        buffer = (SharedBuffer*)MapViewOfFile(
            hMapFile,
            FILE_MAP_ALL_ACCESS,
            0, 0,
            sizeof(SharedBuffer)
        );

        if (!buffer)
            throw std::runtime_error("MapViewOfFile failed");

        memset(buffer->Payload, 0, SHARED_BUFFER_SIZE);
        buffer->PayloadHash = 0;
        buffer->PayloadSize = 0;

        hEvent = CreateEventA(NULL, FALSE, FALSE, eventName);
        if (!hEvent)
            throw std::runtime_error("CreateEvent failed");
    }

    ~IPCChannel()
    {
        if (buffer) UnmapViewOfFile(buffer);
        if (hMapFile) CloseHandle(hMapFile);
        if (hEvent) CloseHandle(hEvent);
    }

    void Write(const void* data, size_t size)
    {
        while (buffer->PayloadSize != 0){
            Sleep(16);
        }

        SharedBuffer transport_buffer;
        memcpy(transport_buffer.Payload, data, size);
        for (int i = size; i < SHARED_BUFFER_SIZE; i++) {
            transport_buffer.Payload[i] = 0;
        }
        memcpy(buffer, &transport_buffer, sizeof(SharedBuffer));

        buffer->PayloadHash = fnv1a_hash(transport_buffer.Payload, SHARED_BUFFER_SIZE); // TODO: we shouldn't be hashing this??
        buffer->PayloadSize = (int)size;
        SetEvent(hEvent);
    }
    bool Read(void* outBuf, size_t maxSize)
    {
        DWORD res = WaitForSingleObject(hEvent, 5000);
        if (res != WAIT_OBJECT_0 && res != WAIT_TIMEOUT) {
            return false;
            MessageBoxA(0, "[CLS-CLIENT] wait for single object failed ", "debug", 0);
        }

        int size = buffer->PayloadSize;
        if (size > 0 && size <= maxSize) 
        {
            // TODO: we probably shouldn't be hasing straight off of the shared mem !
            buffer->PayloadSize = 0;
            auto hash = fnv1a_hash(buffer->Payload, SHARED_BUFFER_SIZE);
            if (buffer->PayloadSize != size
            ||  buffer->PayloadHash != hash) {
                memcpy(outBuf, buffer->Payload, size);
                //MessageBoxA(0, "[CLS-CLIENT] read bujffer success ", "debug", 0);
                return true;
            }
            else {
                MessageBoxA(0, "[CLS-CLIENT] hash or size error!! ", "debug", 0);
            }
        }
        return false;
    }
};