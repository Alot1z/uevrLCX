# Resident Evil 7 (RE Engine) - RenderDoc Captures

**Game:** Resident Evil 7 BIOHAZARD  
**Engine:** RE Engine  
**Graphics API:** DirectX 11  
**Capture Purpose:** Matrix extraction and VR integration analysis  

## Capture Requirements

### Baseline Scenes Required
1. **Main Menu Scene** - UI rendering and basic setup
2. **Gameplay Scene** - Main gameplay with camera matrices
3. **Inventory Scene** - UI elements and HUD projection
4. **Combat Scene** - Weapon rendering and camera tracking

### Matrix Analysis Targets
- **View Matrix:** Camera view transformation for VR head tracking
- **Projection Matrix:** FOV and projection settings for VR
- **World Matrix:** Object positioning and world scale
- **Constant Buffer Analysis:** Memory layout and offsets

## Capture Instructions

### Using RenderDoc
1. Launch RE7 with UEVR cross-engine adapter
2. Start RenderDoc capture before entering target scene
3. Navigate to scene and perform typical actions
4. Stop capture and save as `.rdc` file
5. Analyze capture for matrix data and rendering pipeline

### Capture Settings
- **API:** DirectX 11
- **Capture Options:** Full frame capture
- **Performance:** Minimal impact on game performance
- **File Format:** .rdc (RenderDoc capture format)

## Expected Findings

### RE Engine Specific
- **Constant Buffer Slots:** View/projection matrix locations
- **Rendering Pipeline:** Forward rendering with PBR elements
- **Camera System:** First-person camera implementation
- **Memory Layout:** Object hierarchy and component system

### VR Integration Points
- **SwapChain Hooks:** Present function for stereo rendering
- **Device Hooks:** D3D11 device creation and management
- **Rendering Hooks:** Draw calls for VR optimization
- **Camera Hooks:** Matrix updates for VR transformation

## Analysis Workflow

### Phase 1: Capture Collection
- [ ] Main menu scene capture
- [ ] Gameplay scene capture
- [ ] Inventory scene capture
- [ ] Combat scene capture

### Phase 2: Matrix Analysis
- [ ] Identify constant buffer slots
- [ ] Extract view matrix patterns
- [ ] Extract projection matrix patterns
- [ ] Document memory offsets

### Phase 3: Integration
- [ ] Update camera hook points
- [ ] Validate matrix extraction
- [ ] Test VR camera integration
- [ ] Optimize performance

## File Naming Convention

```
re7_main_menu_YYYYMMDD_HHMMSS.rdc
re7_gameplay_YYYYMMDD_HHMMSS.rdc
re7_inventory_YYYYMMDD_HHMMSS.rdc
re7_combat_YYYYMMDD_HHMMSS.rdc
```

## Current Status

**Status:** Awaiting RenderDoc captures  
**Last Updated:** December 19, 2024  
**Next Step:** Capture baseline scenes for matrix analysis  

## Notes

- RE Engine uses modern PBR rendering pipeline
- Camera system is first-person optimized
- Performance optimization required for VR
- Matrix extraction needed for VR integration
