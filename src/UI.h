#pragma once
#include "res/imgui/imgui.h"
#include "res/imgui/imgui_impl_win32.h"
#include "res/imgui/imgui_impl_dx11.h"


void init_imgui(ID3D11Device1* _dx_device, ID3D11DeviceContext1* _dx_device_context, HWND _target_hwnd) {
    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(_target_hwnd);
    ImGui_ImplDX11_Init(_dx_device, _dx_device_context);




}


void DrawGUI(ID3D11Device1* dx_device, ID3D11DeviceContext1* dx_device_context, HWND target_hwnd) {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(544, 576));

    ImGui::Begin("Sample copy window", NULL, ImGuiWindowFlags_NoInputs);
    ImGui::Text("sample test line 1");
    ImGui::Text("sample test line 2");
    ImGui::End();

    ImGui::Begin("Config UIs");
    ImGui::Text("sample test line 3");
    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}