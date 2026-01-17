#pragma once

#include "res/imgui/imgui.h"
#include "res/imgui/imgui_impl_win32.h"
#include "res/imgui/imgui_impl_dx11.h"


ImFont* style_bigFont;
ImVec4 style_bg = ImVec4(0.0823f, 0.1529f, 0.2156f, 1.0f);
ImVec4 style_accent = ImVec4(0.1725f, 0.2705f, 0.3568f, 1.0f);
ImVec4 style_text = ImVec4(0.764f, 0.764f, 0.764f, 1.0f);

void UI_Init(ID3D11Device1* _dx_device, ID3D11DeviceContext1* _dx_device_context, HWND _target_hwnd) {
    // setup curl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(_target_hwnd);
    ImGui_ImplDX11_Init(_dx_device, _dx_device_context);


    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.ScrollbarRounding = 6.0f;

    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(10, 6);
    style.WindowPadding = ImVec2(12, 12);
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f); // center buttons


    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = style_bg;
    ImGui::GetStyle().Colors[ImGuiCol_Button] = style_accent;
    ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.6f, 1.0f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImVec4(0.2f, 0.4f, 0.8f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_Text] = style_text;

    style.Colors[ImGuiCol_Border] = style_accent;
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0.7f);


    io.Fonts->AddFontDefault();
    style_bigFont = io.Fonts->AddFontDefault();
    style_bigFont->Scale = 1.6f;

}