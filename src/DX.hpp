#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <vector>

class DX
{
public:
    DX();
    ~DX();

    void init();
    HANDLE getSharedHandle();
    ID3D11Texture2D* getTexture() { return m_texture; }

private:
    void createDevice();
    void createTextures();

    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;

    ID3D11Texture2D* m_texture;
};