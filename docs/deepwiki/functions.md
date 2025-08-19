# Functions & Modules Index

Key headers in `include/uevr/`:
- `ICrossEngineAdapter.hpp` — base interface for adapters
- `adapters/common/AdapterFactory.hpp` — factory for engine adapters
- `adapters/common/CrossEngineAdapter.hpp` — shared base implementation
- `adapters/common/CrossEngineAdapterRegistry.hpp` — registration/lookup
- `adapters/common/EngineHooks.hpp` — engine-agnostic hook contracts
- `hooks/*.hpp` — `D3D11Hook`, `D3D12Hook`, `OpenGLHooks`, `VulkanHooks`, `HookManager`
- `vr/OpenXRIntegration.hpp` — VR runtime bridge
- `Plugin.hpp`, `Framework.hpp`, `API.hpp` — integration surface

See source counterparts in `src/` for implementations (e.g., `src/hooks/D3D11Hook.cpp`).
