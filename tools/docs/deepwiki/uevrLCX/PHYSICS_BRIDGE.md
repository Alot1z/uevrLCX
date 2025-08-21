# Physics Bridge (Collision)

## Strategy
- Prefer native game physics: UE PhysX/Chaos, Unity PhysX, Bullet/Havok when present.
- Expose: raycast, sweep, overlap; character controller step/slide.

## Symbol Resolution
- UE: exported names + pattern signatures.
- Unity: IL2CPP/Mono exports; address through metadata registration.
- Custom/Legacy: pattern libraries per title.

## Safety & Scale
- Respect world scale; calibrate `physicsScale` in profile.
- Fail-safe: if no bridge, fall back to minimal internal colliders (haptics only).

## Engine-Specific Notes
- Unity: call `Physics.Raycast/Sweep/Overlap` via managed or IL2CPP-resolved entry points; maintain 1 unit = 1 meter unless title overrides.
- Unreal: expose PhysX/Chaos queries via engine exports; sync world-to-meters and collision channels.

## Interaction Layers
- Map engine layers/masks to VR interaction masks; per-title overrides in `profiles/*/config.json`.

## Haptics
- Trigger on contact; pulse strength by impulse/contact time.

## Character Controllers & Movement
- Kinematic capsule driven by sweep tests for locomotion; handle step offset and crouch scaling.
- Controllers use small trigger colliders and ray/sphere casts for proximity and grab tests.

## Failover Strategy
- If native physics is inaccessible, disable world collision and keep haptic-only interactions; notify via diagnostics.
