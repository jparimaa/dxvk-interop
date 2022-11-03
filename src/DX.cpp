#include "DX.hpp"
#include "Utils.hpp"
#include <comdef.h>
#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <iostream>

namespace
{
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
    releaseDXPtr(m_texture);
    releaseDXPtr(m_deviceContext);
    releaseDXPtr(m_device);
}

void DX::init()
{
    createDevice();
    createTextures();
}

HANDLE DX::getSharedHandle()
{
    HANDLE handle;
    IDXGIResource1* resource;
    HRESULT hr = m_texture->QueryInterface(__uuidof(IDXGIResource1), (void**)&resource);
    checkHresult(hr);
    hr = resource->CreateSharedHandle(NULL,
                                      DXGI_SHARED_RESOURCE_READ,
                                      NULL,
                                      &handle);
    checkHresult(hr);

    resource->Release();
    return handle;
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
    m_deviceContext->Flush();

    /*
    ID3D11Texture2D* tex;
    ID3D11Device1* device1;
    m_device->QueryInterface(__uuidof(ID3D11Device1), (void**)&device1);
    {
        ID3D11Resource* tempResource11;
        HRESULT result = device1->OpenSharedResource1(getSharedHandle(), __uuidof(ID3D11Resource), (void**)&tempResource11);

        if (FAILED(result))
        {
            abort();
        }

        //result = tempResource11->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&tex);
        tempResource11->QueryInterface<ID3D11Texture2D>(&tex);
        tempResource11->Release();

        if (FAILED(result))
        {
            abort();
        }

        D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
        rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        rtvd.Texture2D.MipSlice = 0;
        ID3D11RenderTargetView* copy_tex_view = 0;
        hr = m_device->CreateRenderTargetView(tex, &rtvd, &copy_tex_view);
        checkHresult(hr);
        

        FLOAT clear_color[4] = {1, 1, 1, 1};
        m_deviceContext->ClearRenderTargetView(copy_tex_view, clear_color);
    }
		*/
}
