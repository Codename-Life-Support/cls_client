#pragma once




ComPtr<ID3D11ShaderResourceView> halo_mods_image;



// Loads an image file into an ID3D11ShaderResourceView*
bool LoadTextureFromFile(ID3D11Device* device, ID3D11DeviceContext* context, const std::string& filename, ID3D11ShaderResourceView** out_srv){
    if (!device || !context || !out_srv) {
        MessageBoxA(0, "Invalid arguments to LoadTextureFromFile", "DX texture fail", 0);
        return false;
    }

    int width = 0, height = 0, channels = 0;
    unsigned char* imageData = stbi_load(filename.c_str(), &width, &height, &channels, 4); // Force RGBA

    if (!imageData) {
        const char* reason = stbi_failure_reason();

        char buffer[512];
        snprintf(buffer, sizeof(buffer),
            "Failed to load texture:\n%s\n\nReason:\n%s",
            filename.c_str(),
            reason ? reason : "Unknown error");

        MessageBoxA(nullptr, buffer, "DX texture failed to load", MB_ICONERROR);
        return false;
    }


    // Describe texture
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;

    // Fill subresource data
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = imageData;
    initData.SysMemPitch = width * 4; // 4 bytes per pixel (RGBA)

    ComPtr<ID3D11Texture2D> texture;
    HRESULT hr = device->CreateTexture2D(&texDesc, &initData, &texture);
    stbi_image_free(imageData); // Free CPU image data after upload

    if (FAILED(hr)) {
        MessageBoxA(0, "Failed to create texture", "DX texture fail", 0);
        return false;
    }

    // Create Shader Resource View
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(texture.Get(), &srvDesc, out_srv);
    if (FAILED(hr)) {
        MessageBoxA(0, "Failed to create ShaderResourceView", "DX texture fail", 0 );
        return false;
    }

    return true;
}

void DX_TexturesInit(ID3D11Device* device, ID3D11DeviceContext* context) {

    if (LoadTextureFromFile(device, context, "halomods.png", &halo_mods_image)) {

    }
}
// Example usage
int main() {



    return 0;
}
