# uevrLCX – Meta Quest 3 PCVR (OpenXR) + 2D→VR Aesthetics/Collision

Goal: deliver true stereoscopic VR (two eyes, depth, 6DoF) on Meta Quest 3 via PC (Link/Air Link) using OpenXR, and provide a realistic strategy to transform 2D games into a VR experience with "aesthetics" and basic "collision".

---------------------------------------------------------------------------------------------------

## 1) Scope and realities

- This is PCVR:
  - The game runs on the PC.
  - Quest 3 uses the Oculus desktop OpenXR runtime (Link/Air Link).
  - Converting PC games to Quest 3 standalone APKs is not realistic in this context.

- True 3D VR needs:
  - OpenXR frame loop: xrWaitFrame → xrBeginFrame → xrLocateViews → per‑eye render → xrEndFrame.
  - Per‑eye view/projection and swapchains.
  - Engine hooks that render to XR targets instead of a monoscopic backbuffer.

- 2D→VR (e.g., Mario): a realistic pipeline exists, but it isn’t automatic magic. See section 8.

References (Context7/DeepWiki ready):
- Khronos OpenXR Spec: /khronosgroup/openxr-docs
- Khronos OpenXR SDK: /khronosgroup/openxr-sdk
- Meta OpenXR SDK (Quest): /meta-quest/meta-openxr-sdk

---------------------------------------------------------------------------------------------------

## 2) System requirements (PCVR)

- PC (Windows 10/11 64‑bit)
  - CPU: 6+ cores (e.g., Ryzen 5 5600 / Intel 12400)
  - GPU min: RTX 2060 / RX 5700; for 90 Hz: RTX 3060 Ti / RX 6700 XT+
  - RAM: 16 GB+
  - Link (USB 3.0) or Air Link (Wi‑Fi 6/6E)
- Quest 3 + Oculus desktop app
  - In Oculus app: Settings → OpenXR → "Set Oculus as active OpenXR runtime"
- Tooling: Visual Studio 2022, CMake, Windows SDK
- CMake flags: `-Duevr_ENABLE_OPENXR=ON` (start without OpenVR)

---------------------------------------------------------------------------------------------------

## 3) Build fixes (CMake) – minimal and upstream‑friendly

- `src/vr/CMakeLists.txt`: add `VRSystem.cpp` to `uevr_vr`.
- `src/mods/CMakeLists.txt`: include `VRModManager.cpp` only if the file exists.

Patches (apply manually; these are documentation snippets, not auto‑writes):

*** Begin Patch
*** Update File: src/vr/CMakeLists.txt
@@
 add_library(uevr_vr STATIC
     UniversalVRSystem.cpp
     EngineAgnosticRenderer.cpp
+    VRSystem.cpp
 )
*** End Patch

*** Begin Patch
*** Update File: src/mods/CMakeLists.txt
@@
-# Mod sources
-set(MOD_SOURCES
-    VRModManager.cpp
-    CameraMod.cpp
-    ControllerMod.cpp
-    DisplayMod.cpp
-    AudioMod.cpp
-    PerformanceMod.cpp
-)
+# Mod sources
+set(MOD_SOURCES
+    CameraMod.cpp
+    ControllerMod.cpp
+    DisplayMod.cpp
+    AudioMod.cpp
+    PerformanceMod.cpp
+)
+# Include VRModManager.cpp only if it exists (prevents missing-file build errors)
+if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/VRModManager.cpp")
+    list(APPEND MOD_SOURCES VRModManager.cpp)
+endif()
*** End Patch

---------------------------------------------------------------------------------------------------

## 4) Architecture in uevrLCX

- VR runtime manager: `src/vr/VRSystem.cpp`, `src/vr/VRSystem.h`
  - OpenXR init/system/session/space
  - Swapchains (D3D11 first; D3D12 later)
  - Frame loop
  - Per‑eye view/projection
  - Input actions (phase 2)
  - Submit layers

- Hooks: `src/hooks/D3D11Hook.cpp`, `src/hooks/D3D12Hook.cpp`
  - Intercept render/present
  - Replace monoscopic backbuffer with XR swapchain image targets per eye
  - Call `VRSystem` for view/projection and frame submission

- Adapters/engine integration: `src/adapters/…`, `src/core/…`
  - Override FOV/clip planes/world scale per engine/title
  - UI in VR (projection/orientation)

---------------------------------------------------------------------------------------------------

## 5) Function map (aligned with `src/vr/VRSystem.h`)

Already declared:
- System: `initializeVRSystem()`, `cleanupVRSystem()`
- Mode: `activateVRMode()`, `deactivateVRMode()`, `isVRModeActive()`
- Runtimes: `initializeOpenXR()`, `initializeOpenVR()`, `initializeSteamVR()`, `getCurrentVRRuntime()`
- Devices: `detectVRHeadset()`, `getHeadsetName()`, `detectVRControllers()`, `getControllerCount()`
- Rendering: `initializeVRStereoRendering()`, `setVRViewport(int w,int h)`, `submitVRFrame()`, `presentVRFrame()`
- Input: `initializeVRInput()`, `pollVRInput()`, `getControllerPose()`, `getControllerButtonState()`
- Performance: `enableAsyncReprojection()`, `enableMotionSmoothing()`, `setVRPerformanceMode(const std::string&)`

Suggested OpenXR internals (inside `VRSystem.cpp`):
- Session/loop:
  - `bool createXRSessionAndSpaces();`
  - `bool waitBeginFrame(XrFrameState& outState);`
  - `bool endFrame(const XrFrameState& state);`
- Views:
  - `bool locateStereoViews(const XrFrameState& state);`
  - `Matrix4 getEyeViewMatrix(int eye);`
  - `Matrix4 getEyeProjection(int eye);`
- Swapchains:
  - `bool createSwapchainsD3D11(ID3D11Device* dev, const ViewConfig& vc);`
  - `bool acquireEyeImage(int eye, uint32_t& outIndex);`
  - `bool releaseEyeImage(int eye);`

Names/types can be adapted to your math layer; the key is to have clear entry points for the loop, views, and swapchains.

---------------------------------------------------------------------------------------------------

## 6) OpenXR – init and frame loop (authoritative steps)

1) Instance/system:
- `xrCreateInstance` (+ required extensions)
- `xrGetSystem(XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY)`

2) Graphics binding + session:
- D3D11 (or D3D12) graphics binding for `xrCreateSession`
- Choose `XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO`
- Create reference space (`LOCAL` or `STAGE`)

3) Swapchains:
- `xrEnumerateViewConfigurationViews` → recommended resolution per eye
- `xrEnumerateSwapchainFormats` → choose color format (prefer sRGB, e.g., `DXGI_FORMAT_R8G8B8A8_UNORM_SRGB`)
- Create color swapchain with `arraySize = 2` (or two separate). Depth swapchain recommended.

4) Per‑frame:
- `xrWaitFrame` → `xrBeginFrame`
- `xrLocateViews` with `predictedDisplayTime` → `XrView views[2]`
- For each eye:
  - Acquire + Wait swapchain image
  - Bind as RTV/DSV; render with per‑eye view/projection
  - Release image
- Build `XrCompositionLayerProjectionView` per eye and a single `XrCompositionLayerProjection`
- `xrEndFrame`

Key details:
- sRGB color space and gamma correctness
- D3D depth range 0..1 → proper projection
- MSAA often unsupported on XR swapchains; render to MSAA RT and resolve
- World scale: 1 meter in VR matches game world (UE‑like: 1 UU = 1 cm → scale 0.01)

---------------------------------------------------------------------------------------------------

## 7) Hook integration (true stereo)

Focus: `src/hooks/D3D11Hook.cpp` (D3D12 later)

- Init:
  - Obtain `ID3D11Device*` and `ID3D11DeviceContext*` from the game (hook already does)
  - Call `VRSystem.createSwapchainsD3D11(...)` with the device

- Per frame:
  - `VRSystem.waitBeginFrame(state)`
  - `VRSystem.locateStereoViews(state)`
  - For eye in {Left, Right}:
    - `VRSystem.acquireEyeImage(eye, idx)`
    - Create/cache RTV/DSV from swapchain image (once, reuse)
    - Bind RTV/DSV; set viewport from XR view size
    - Render scene with `getEyeViewMatrix(eye)` and `getEyeProjection(eye)`
    - `VRSystem.releaseEyeImage(eye)`
  - `VRSystem.endFrame(state)`

- UI/overlays:
  - Start with a world‑locked flat quad UI
  - Later: diegetic UI per title/adapter

---------------------------------------------------------------------------------------------------

## 8) 2D→VR Aesthetics + "Collision" (Mario‑style)

Goal: For 2D pixel games, build a VR experience with depth and basic interaction. A generic solution needs trade‑offs:

A. Emulator‑driven scene reconstruction (most robust)
- Environment:
  - Run the 2D game in an emulator (e.g., RetroArch cores: NES/SNES/GB/GBA…)
  - Write a plugin/bridge to read graphical state:
    - Tilemap/nametable, OAM (sprite metadata), palette, scroll offset
- Reconstruction:
  - Lay the tilemap on a 3D plane (grid of quads). Light extrusion (2.5D)
  - Sprites: place as plane billboards in Z layers (parallax)
  - Depth: heuristic (background/foreground layers) or manual rules per game/level
- "Collision":
  - Use the emulator’s collision/hit events and OAM AABBs to drive simple VR interactions
  - Map VR controller “touch” to game inputs, not engine physics (unless emulator exposes hooks)
- VR:
  - Camera: HMD pose to view a “diorama”
  - Controllers: raycast against designated interactive layers (mapped to A/B/Start/Select/DPAD)
- Pros: deterministic, low latency, real game data
- Cons: emulator‑specific hooks; per‑platform work

B. Computer‑vision depth (quick PoC, less robust)
- Estimate monocular depth per frame (e.g., MiDaS‑like) – CPU/GPU heavy
- Lift the 2D frame to a view‑dependent 3D mesh (image‑space warping)
- "Collision": pseudo only – define mask‑based hit regions (color/edges)
- Pros: no emulator integration
- Cons: artifacts, latency, unreliable depth

C. Parallax layering (manual/heuristic)
- Split the image into layers (background/mid/foreground) and place them in Z
- No real collision, only aesthetics
- Easy to implement, not generically interactive

Practical plan for 2D:
- Short term: “VR theater” + parallax (C)
- Next: Emulator plugin (A) for 2.5D scene and OAM‑based interactions
- CV depth (B) can be tested separately as an experiment (not production‑ready)

Note:
- Real “collision” inside the game logic requires access to the game state. For console 2D games, the emulator state is the best source.
- For PC 2D games without source, you end up with memory scanning/overlay interaction – not 100% robust.

---------------------------------------------------------------------------------------------------

## 9) Testing – Quest 3 (PCVR)

- Oculus desktop → OpenXR runtime = Oculus
- Link/Air Link enabled
- Build with `-Duevr_ENABLE_OPENXR=ON`
- Logs (examples):
  - "OpenXR instance/system/session created"
  - "View config: PRIMARY_STEREO"
  - "Swapchain format(s): …, size: WxH"
  - "Frame: Wait/Begin/Locate/End ok"
- Verify in HMD:
  - True depth (parallax), correct scale/FOV/clip, no double images
- Performance:
  - Start with lower render scale (XR recommended size)
  - Target 72/80/90 Hz; tune per title

---------------------------------------------------------------------------------------------------

## 10) Performance and comfort

- Foveation (later): Meta `XR_FB_foveation`
- Space warp (later): `XR_FB_space_warp` (needs motion vectors)
- Comfort:
  - Snap turn, vignette, pitch clamp
  - Render mask (fixed foveation) to reduce fill

---------------------------------------------------------------------------------------------------

## 11) Action plan (short, aligned with TODO)

- Build fixes:
  - Add `VRSystem.cpp` in `src/vr/CMakeLists.txt`
  - Guard `VRModManager.cpp` in `src/mods/CMakeLists.txt`
- OpenXR frame loop in `VRSystem.cpp`:
  - Init/session/space, swapchains, per‑frame loop, submit
- D3D11 hook:
  - Render per eye to XR swapchain RTVs, not the backbuffer
- Input (phase 2):
  - Action set + controller bindings; map to game input
- Test with Quest 3:
  - Oculus runtime active, Link/Air Link, build runs
- 2D→VR PoC:
  - Start “VR theater + parallax”
  - Plan emulator‑plugin prototype for tiles/sprites (A)

---------------------------------------------------------------------------------------------------

## 12) Key repo files

- `src/vr/VRSystem.cpp`, `src/vr/VRSystem.h`
- `src/vr/CMakeLists.txt`
- `src/hooks/D3D11Hook.cpp`, `src/hooks/D3D12Hook.cpp`
- `src/mods/CMakeLists.txt`
- Tests: `tests/CMakeLists.txt`

---------------------------------------------------------------------------------------------------

## 13) References

- Khronos OpenXR Specification: /khronosgroup/openxr-docs
- Khronos OpenXR SDK: /khronosgroup/openxr-sdk
- Meta OpenXR SDK (Quest): /meta-quest/meta-openxr-sdk
- DeepWiki: use to read repo overviews and topics like "projection layer", "swapchain", "view location" for the correct structs and fields.
