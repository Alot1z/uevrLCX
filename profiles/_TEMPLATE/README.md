# Game Profile Template

This is a template profile for UEVR cross-engine VR support. Copy this directory and customize it for your specific game.

## Profile Structure

- `profile.json` - Main profile configuration
- `README.md` - This documentation file
- `screenshots/` - VR screenshots and comparison images
- `configs/` - Additional configuration files
- `patches/` - Game-specific patches or mods

## Configuration Guide

### 1. Basic Information
Update the `game_info` section with your game's details:
- `executable`: Game executable filename
- `display_name`: Human-readable game name
- `steam_app_id`: Steam App ID if available
- `epic_app_id`: Epic Games App ID if available

### 2. Engine Detection
Set the `engine` field to match your game's engine:
- `RE-Engine` - Capcom games (Resident Evil, Monster Hunter)
- `REDengine4` - CD Projekt games (Cyberpunk 2077, Witcher 3)
- `MT-Framework` - Capcom legacy games
- `Unreal-Engine-4/5` - Unreal Engine games
- `Unity-Engine` - Unity games
- `Custom-Engine` - Other engines

### 3. Rendering Configuration
Configure the `render` section based on your game:
- `api`: Graphics API (d3d11, d3d12, vulkan, opengl)
- `matrix_sources`: View and projection matrix locations
- `hook_points`: Functions to hook for VR rendering

### 4. VR Features
Enable/disable VR features in the `vr_features` section:
- `stereo_rendering`: Enable stereo rendering
- `motion_controllers`: Enable motion controller support
- `haptic_feedback`: Enable haptic feedback
- `ray_tracing`: Enable ray tracing support (if available)

### 5. Performance Tuning
Adjust performance settings in the `performance` section:
- `resolution_scale`: VR resolution scaling
- `uncap_framerate`: Remove framerate limits
- `async_reprojection`: Enable async reprojection

## Testing and Validation

1. **Basic VR**: Verify stereo rendering works
2. **Input**: Test motion controller input
3. **Performance**: Check framerate and stability
4. **Comfort**: Ensure comfortable VR experience

## Troubleshooting

- Check console output for error messages
- Verify matrix sources are correct
- Test with different VR headsets
- Check game compatibility notes

## Contributing

When contributing a profile:
1. Test thoroughly with the target game
2. Document any special requirements
3. Include screenshots of VR implementation
4. Update compatibility tier based on testing

## Support

For issues with this profile:
1. Check the main UEVR documentation
2. Search existing issues
3. Create a new issue with detailed information
4. Include game version and VR headset details
