# Graphics Hooks

- __D3D11__: IDXGISwapChain::Present/ResizeBuffers, VS/PS constant buffers for view/projection.
- __D3D12__: ID3D12CommandQueue::ExecuteCommandLists, swapchain present path, root‑signature bound CBVs.
- __Vulkan__: swapchain creation/present, command buffer intercepts, UBO matrix extraction.

References (authoritative):
- Microsoft Docs — DXGI Present: https://learn.microsoft.com/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-present
- Microsoft Docs — D3D12 ExecuteCommandLists: https://learn.microsoft.com/windows/win32/api/d3d12/nf-d3d12-id3d12commandqueue-executecommandlists
- Vulkan Spec: https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap50.html
- RenderDoc Guide: https://renderdoc.org/docs
- MinHook: https://github.com/TsudaKageyu/minhook

See: `src/hooks/`, `include/uevr/hooks/`, and engine pages in this wiki.
