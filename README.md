# dxvk-interop

An example how to do interop between DirectX 11 and Vulkan. This example uses the import-method, i.e. [VkImportMemoryWin32HandleInfo](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkImportMemoryWin32HandleInfoKHR.html), so DirectX 11 texture is imported to Vulkan. It would also be possible to [export](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkExportMemoryWin32HandleInfoKHR.html) memory from Vulkan to DirectX.

Steps:
- Enable instance and device extensions: `VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME` and `VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME`
- Create DX11 texture with `MiscFlags = D3D11_RESOURCE_MISC_SHARED_NTHANDLE | D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX`
- Create shared handle and keyed mutex: `IDXGIResource1::CreateSharedHandle` and `m_texture->QueryInterface(__uuidof(IDXGIKeyedMutex), (LPVOID*)&m_keyedMutex)`
- Setup Vulkan and initialize image memory `pNext` with `VkImportMemoryWin32HandleInfoKHR` using the shared handle from above.
- Render to DX texture between `IDXGIKeyedMutex::AcquireSync` and `IDXGIKeyedMutex::ReleaseSync`
