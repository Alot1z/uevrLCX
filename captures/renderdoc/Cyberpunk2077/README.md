# Cyberpunk 2077 RenderDoc Captures

## Directory Purpose
This directory contains RenderDoc (.rdc) capture files for analyzing Cyberpunk 2077's REDengine 4 rendering pipeline. These captures are essential for understanding the engine's rendering behavior and implementing proper VR hooks.

## Required Captures

### Baseline Capture
**File:** `cp2077_baseline.rdc`
**Purpose:** Establish baseline rendering pipeline analysis
**Capture Point:** Main menu or early game scene
**Requirements:**
- Game running at stable 60+ FPS
- No VR mode active (flat screen only)
- Standard graphics settings
- Capture duration: 30-60 seconds

### VR Integration Capture
**File:** `cp2077_vr_integration.rdc`
**Purpose:** Verify VR rendering pipeline integration
**Capture Point:** VR mode active with stereo rendering
**Requirements:**
- VR mode enabled
- Stereo rendering active
- Both eyes rendering correctly
- Capture duration: 30-60 seconds

### Performance Capture
**File:** `cp2077_performance.rdc`
**Purpose:** Analyze performance bottlenecks in VR
**Capture Point:** Complex scene with multiple objects
**Requirements:**
- High object count scene
- VR mode active
- Performance monitoring enabled
- Capture duration: 60+ seconds

## Capture Instructions

### 1. Baseline Capture Setup
1. **Launch Cyberpunk 2077** in flat screen mode
2. **Start RenderDoc** and attach to the game process
3. **Navigate to main menu** or early game scene
4. **Begin capture** and let it run for 30-60 seconds
5. **Save capture** as `cp2077_baseline.rdc`

### 2. VR Integration Capture Setup
1. **Enable VR mode** through UEVR
2. **Verify stereo rendering** is working
3. **Navigate to same scene** as baseline
4. **Begin capture** and let it run for 30-60 seconds
5. **Save capture** as `cp2077_vr_integration.rdc`

### 3. Performance Capture Setup
1. **Navigate to complex scene** (city, crowded area)
2. **Ensure VR mode** is active and stable
3. **Begin capture** and let it run for 60+ seconds
4. **Save capture** as `cp2077_performance.rdc`

## Analysis Requirements

### Baseline Analysis
- **Rendering Pipeline:** Identify D3D12 command structure
- **Camera Matrices:** Extract view and projection matrices
- **Object Rendering:** Understand object hierarchy
- **Memory Patterns:** Identify key memory structures

### VR Integration Analysis
- **Stereo Rendering:** Verify left/right eye separation
- **Camera Transformation:** Check VR camera integration
- **Performance Impact:** Measure VR overhead
- **Hook Stability:** Verify hook installation

### Performance Analysis
- **Frame Timing:** Analyze frame pacing
- **Memory Usage:** Monitor memory allocation
- **GPU Utilization:** Identify bottlenecks
- **VR Optimization:** Find optimization opportunities

## File Naming Convention
- **Baseline:** `cp2077_baseline.rdc`
- **VR Integration:** `cp2077_vr_integration.rdc`
- **Performance:** `cp2077_performance.rdc`
- **Debug:** `cp2077_debug_[description].rdc`

## Validation Checklist

### Baseline Capture ✅
- [ ] Game running at stable FPS
- [ ] No VR mode active
- [ ] Standard graphics settings
- [ ] 30-60 second duration
- [ ] File saved as `cp2077_baseline.rdc`

### VR Integration Capture ⏳
- [ ] VR mode enabled
- [ ] Stereo rendering active
- [ ] Both eyes rendering
- [ ] 30-60 second duration
- [ ] File saved as `cp2077_vr_integration.rdc`

### Performance Capture ⏳
- [ ] Complex scene selected
- [ ] VR mode active
- [ ] Performance monitoring
- [ ] 60+ second duration
- [ ] File saved as `cp2077_performance.rdc`

## Notes
- **Capture Quality:** Ensure captures are representative of normal gameplay
- **File Size:** Large captures may be compressed or split
- **Analysis Tools:** Use RenderDoc's built-in analysis tools
- **Documentation:** Record findings in analysis reports

## Next Steps
1. **Complete baseline capture** for initial analysis
2. **Implement VR integration** based on findings
3. **Capture VR integration** for validation
4. **Analyze performance** and optimize
5. **Document all findings** in technical reports
