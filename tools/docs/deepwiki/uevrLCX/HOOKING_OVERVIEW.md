# Hooking Overview (uevrLCX)

This DeepWiki section summarizes hooking strategies for different render APIs and engines.

## Render API Hooks
- DX11/DX12: swapchain, present, resize, command lists; depth buffer discovery.
- DX9/10: device vtable hooks; limited modern features.
- OpenGL: wglSwapBuffers/glXSwapBuffers; FBO detection.
- Vulkan: vkQueuePresentKHR; renderpass/framebuffer tracking.

## Engine Entry Points
- Unreal: `UWorld::Tick`, `APlayerController::CalcCamera`, render thread setup.
- Unity: `Camera.Render`, `Transform` updates; IL2CPP/Mono symbol mapping.
- Legacy/Custom: identify main loop and render submission functions; pattern sets per title.

## Runtime Integration
- Use `uevr::vr::IStereoRenderer` to abstract render API specifics (`GraphicsAPI`, `StereoMode`).
- Schedule frames with `uevr::vr::IFrameScheduler` for pacing/skip decisions.

## Submission Points
- OpenXR/OpenVR runtime submission occurs after per-eye rendering; API-specific details are encapsulated by the runtime layer.

See also: `ENGINE_UE_ADAPTER.md`, `ENGINE_UNITY_ADAPTER.md`, `RENDERING_PATHS.md`.
