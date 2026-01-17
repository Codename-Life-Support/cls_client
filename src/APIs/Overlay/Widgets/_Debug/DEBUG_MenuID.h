#pragma once


typedef void*(OrdinalCall)();
typedef int*(OrdinalToggle)(int RCX);

OrdinalToggle* OrdinalTog; // used to determine which ordinal we need to call
OrdinalCall* Ordinal_13;
OrdinalCall* Ordinal_60;

uint64_t _ordinal_tog = 0;
uint64_t menuid_blob_result = 0;

void DEBUG_menuid() {
    menuid_blob_result = 0;
    int* ordinal_toggle = OrdinalTog(1);
    _ordinal_tog = (uint64_t)ordinal_toggle;
    if (*ordinal_toggle == 0) {
        menuid_blob_result = (uint64_t)Ordinal_60();
    }
    else {
        menuid_blob_result = (uint64_t)Ordinal_13();
    }
}
/* HOOKS
* 
    // menu info
    Ordinal_13 = (OrdinalCall*)GetProcAddress(networkshipping_module, (LPCSTR)13);
    Ordinal_60 = (OrdinalCall*)GetProcAddress(networkshipping_module, (LPCSTR)60);
    OrdinalTog = (OrdinalToggle*)((char*)exeModule + 0x34CEF8);
    if (!Ordinal_13) {
        MessageBoxA(0, "[CLS-CLIENT] failed to find Ordinal_13", "Network hook failure", 0);
        return false;
    }
    if (!Ordinal_60) {
        MessageBoxA(0, "[CLS-CLIENT] failed to find Ordinal_60", "Network hook failure", 0);
        return false;
    }
*/

/* UI
    ImGui::Begin("Menu Debug");
    sprintf_s(hexbuf, "0x%016llX", menuid_blob_result);
    ImGui::InputText("Ptr Result", hexbuf, sizeof(hexbuf), ImGuiInputTextFlags_ReadOnly);
    sprintf_s(hexbuf, "0x%016llX", _ordinal_tog);
    ImGui::InputText("ordinal_toggle", hexbuf, sizeof(hexbuf), ImGuiInputTextFlags_ReadOnly);

    if (ImGui::Button("attempt#123")) {
        DEBUG_menuid();
    }

    ImGui::End();

*/