# Unreal Engine Adapter Notes

- Hook: `UWorld::Tick`, `APlayerController::CalcCamera`, render command setup.
- Physics: PhysX/Chaos raycast/sweep/overlap bridge; world-to-meters scaling.
- Rendering: per-eye VP; handle TAA jitter; HUD to OpenXR quad layers.
- Docs: https://docs.unrealengine.com/ | VR: https://docs.unrealengine.com/5.0/en-US/virtual-reality-development-in-unreal-engine/
