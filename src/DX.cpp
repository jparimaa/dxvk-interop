#include "DX.hpp"
#include "Utils.hpp"
#include <comdef.h>
#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <iostream>

namespace
{
float clearBlue = 1.0f;

template<typename T>
void releaseDXPtr(T*& ptr)
{
    if (ptr != nullptr)
    {
        ptr->Release();
        ptr = nullptr;
    }
}

void checkHresult(HRESULT hr)
{
    if (FAILED(hr))
    {
        std::cerr << "HRESULT error: " << _com_error(hr).ErrorMessage() << "\n";
        abort();
    }
}
} // namespace

DX::DX()
{
}

DX::~DX()
{
    releaseDXPtr(m_rtv);
    releaseDXPtr(m_dxgiMutex);
    releaseDXPtr(m_texture);
    releaseDXPtr(m_deviceContext);
    releaseDXPtr(m_device);
}

void DX::init()
{
    createDevice();
    createTextures();
    createSharedObjects();
}

void DX::update()
{
    const UINT64 acqKey = 0;
    const UINT64 relKey = 0;
    const DWORD timeOutInMs = 5;
    HRESULT result = m_dxgiMutex->AcquireSync(acqKey, timeOutInMs);
    if (result == WAIT_OBJECT_0)
    {
        clearBlue = clearBlue < 0.0f ? 1.0f : clearBlue - 0.0003f;
        float clearColor[4] = {0.0f, 0.0f, clearBlue, 1.0f};
        m_deviceContext->ClearRenderTargetView(m_rtv, clearColor);
    }
    result = m_dxgiMutex->ReleaseSync(relKey);
    checkHresult(result);
}

HANDLE DX::getSharedHandle()
{
    return m_sharedHandle;
}

void DX::createDevice()
{
    UINT flags = 0;
#ifdef _DEBUG
    flags = D3D11_CREATE_DEVICE_DEBUG;
#endif
    const HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, flags, nullptr, 0, D3D11_SDK_VERSION, &m_device, nullptr, &m_deviceContext);
    checkHresult(hr);
}

void DX::createTextures()
{
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = c_texWidth;
    desc.Height = c_texHeight;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET;
    desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_NTHANDLE | D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

    const UINT rowSizeInBytes = c_texWidth * c_texChannels;
    const UINT imageSizeInBytes = c_texWidth * c_texHeight * c_texChannels;

    std::vector<uint8_t> imageData(imageSizeInBytes, 128);
    for (uint32_t i = 0; i < imageSizeInBytes; i += 4)
    {
        imageData[i + 0] = i % 200 + 20;
        imageData[i + 1] = 255 - (i % 255);
        imageData[i + 2] = 128 + (i % 127);
        imageData[i + 3] = 255;
    }

    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = reinterpret_cast<void*>(imageData.data());
    initData.SysMemPitch = rowSizeInBytes;
    initData.SysMemSlicePitch = imageSizeInBytes;

    HRESULT hr = m_device->CreateTexture2D(&desc, &initData, &m_texture);
    checkHresult(hr);

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;
    hr = m_device->CreateRenderTargetView(m_texture, &rtvDesc, &m_rtv);
    checkHresult(hr);
}

void DX::createSharedObjects()
{
    IDXGIResource1* dxgiResource;
    HRESULT hr = m_texture->QueryInterface(__uuidof(IDXGIResource1), (void**)&dxgiResource);
    checkHresult(hr);
    hr = dxgiResource->CreateSharedHandle(NULL, DXGI_SHARED_RESOURCE_READ, NULL, &m_sharedHandle);
    checkHresult(hr);

    dxgiResource->Release();

    hr = m_texture->QueryInterface(__uuidof(IDXGIKeyedMutex), (LPVOID*)&m_dxgiMutex);
    checkHresult(hr);
}
