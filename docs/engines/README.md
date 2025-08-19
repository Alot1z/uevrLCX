# UEVR Engine Support Documentation

This directory contains technical specifications and implementation details for all supported game engines in the UEVR Universal Engine Deployment system.

## Supported Engine Families

### Tier 1 Engines (Full Support)

#### REDengine 4 (CD Projekt RED)
- **Primary Target**: Cyberpunk 2077
- **Adaptation Strategy**: Direct REDengine 4 adapter with custom rendering hooks
- **Additional Games**: The Witcher 3 (REDengine 3 compatibility layer)
- **Engine Flexing**: Custom shader injection, temporal upsampling, neural network integration
- **Documentation**: [REDengine4.md](./REDengine4.md)

#### RE Engine (Capcom)
- **Primary Targets**: 
  - Resident Evil 4 Remake
  - Resident Evil 8: BIOHAZARD
  - Monster Hunter World/Rise
- **Adaptation Strategy**: Universal RE Engine adapter with version detection
- **Additional Games**: Devil May Cry 5, Street Fighter 6, Dragon's Dogma 2
- **Engine Flexing**: Dynamic FOV adjustment, motion-to-photon optimization, haptic feedback integration
- **Documentation**: [RE-Engine.md](./RE-Engine.md)

#### Unreal Engine (Epic Games)
- **Version Support**: UE4.20+ and UE5.x
- **Adaptation Strategy**: Generic UObject manipulation and rendering pipeline hooks
- **Target Games**: 500+ UE4/UE5 titles including Fortnite, Gears series, Borderlands 3
- **Engine Flexing**: Universal blueprint injection, scalable LOD systems, cross-platform compatibility
- **Documentation**: [Unreal-Engine.md](./Unreal-Engine.md)

#### Unity Engine
- **Version Support**: Unity 2019.4 LTS through Unity 6
- **Adaptation Strategy**: IL2CPP and Mono runtime hooking
- **Target Games**: Ori series, Cuphead, Cities: Skylines, thousands of indie titles
- **Engine Flexing**: Component-based VR adaptation, universal shader replacement
- **Documentation**: [Unity-Engine.md](./Unity-Engine.md)

### Tier 2 Engines (Experimental Support)

#### CryEngine/CRYENGINE
- **Version Support**: CryEngine 3.x through CRYENGINE 5.7
- **Target Games**: Hunt: Showdown, Kingdom Come: Deliverance, Crysis series
- **Engine Flexing**: Advanced lighting adaptation, vegetation rendering optimization
- **Documentation**: [CryEngine.md](./CryEngine.md)

#### id Tech Engine
- **Version Support**: id Tech 6/7
- **Target Games**: DOOM (2016), DOOM Eternal, Wolfenstein series
- **Engine Flexing**: High-performance rendering optimization, advanced culling systems
- **Documentation**: [id-Tech.md](./id-Tech.md)

#### Source Engine/Source 2
- **Target Games**: Half-Life: Alyx, Counter-Strike 2, Dota 2
- **Engine Flexing**: Legacy compatibility maintenance, modern VR integration
- **Documentation**: [Source-Engine.md](./Source-Engine.md)

## Engine Detection & Compatibility

### Automatic Detection
Each engine implements automatic detection using:
- Process name analysis
- Module signature scanning
- Memory pattern recognition
- Registry key inspection
- Custom detection functions

### Compatibility Tiers
- **Tier 0**: Not detected/blocked (no hooks)
- **Tier 1**: Boot + basic camera pose (no UI, flat view)
- **Tier 2**: Stereo + head-tracked camera + FOV control
- **Tier 3**: Input remap + HUD transforms + basic 6DoF
- **Tier 4**: Full VR integration (stereo rendering, hands, UI reprojection)
- **Tier 5**: Enhanced features (DLSS/FSR path, neural interpolation, reprojection)

### Engine-Specific Features
Each engine supports different VR features based on their architecture:
- **REDengine 4**: Neural upsampling, ray-tracing fallback
- **RE Engine**: Motion blur adaptation, dynamic FOV
- **Unreal Engine**: Blueprint injection, scalable LOD
- **Unity**: Component-based adaptation, universal shaders

## Implementation Guidelines

### Adapter Development
1. Extend `BaseCrossEngineAdapter` class
2. Implement engine-specific detection methods
3. Configure hook points and matrix sources
4. Test with multiple game versions
5. Document compatibility matrix

### Performance Optimization
- Implement dynamic resolution scaling
- Enable motion reprojection
- Optimize LOD systems
- Support async reprojection

### Safety & Compatibility
- Implement blacklist/kill-switch for risky titles
- Enable offline-only mode by default
- Provide safe-mode for unknown builds
- Validate all hooks before activation

## Testing & Validation

### Automated Testing
- Engine detection validation
- Hook point verification
- Matrix source validation
- Performance benchmarking
- Compatibility tier assessment

### Manual Validation
- RenderDoc frame analysis
- Constant buffer inspection
- Hook point verification
- VR feature testing
- Performance monitoring

## Contributing

To add support for a new engine:
1. Create engine-specific adapter class
2. Implement detection methods
3. Configure hook points and matrices
4. Create engine documentation
5. Add to compatibility matrix
6. Test with multiple games
7. Submit pull request with full documentation

## Resources

- [Adapter Schema](../adapter.schema.json) - Canonical adapter configuration format
- [Hook Patterns](../hooks/) - Universal hooking patterns and safety mechanisms
- [Rendering](../rendering/) - Cross-platform rendering abstraction
- [Compatibility](../compatibility/) - Game-specific adaptation notes
- [Validation](../validation/) - Automated testing and quality assurance
- [Deployment](../deployment/) - Production deployment guidelines
