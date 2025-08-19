DU SKAL KUN LAVE PÅ MAIN RE7 DELE OG INTEGERE DET MAIN CROSS EGNIENF OR AT FÅ DET ITL AT VIRKE
 fra toppen af @promt re7.md 

# Resident Evil 7 (RE Engine) — Focus Area

 ANVEND FULDT FRA ALEL DOCS AMPPERENE DER ANVENDLIGT FOR DET HER 


**Priority Target:** RE Engine components only for this development session.


- **RE Engine Architecture:** Modern Capcom engine analysis
- **Lighting System Integration:** PBR and shadow system VR compatibility  
- **Camera System Reverse Engineering:** First-person VR transformation
- **Performance Profiling:** RE Engine frame timing and rendering bottleneck analysis


**Executable Path:** `G:\SteamLibrary\steamapps\common\RESIDENT EVIL 7 biohazard\re7.exe`
**Profile Target:** `profiles/re7-Win64-Shipping/`
<PERFORM NOW — Cross‑engine adapters for CP2077 / MHW / RE7


You are continuing cross‑engine VR adapter development for EXACTLY these three games only:
- Resident Evil 7 (RE Engine) — exe path: G:\SteamLibrary\steamapps\common\RESIDENT EVIL 7 biohazard\re7.exe
- Cyberpunk 2077 (REDengine 4) — exe: Cyberpunk2077.exe
- Monster Hunter: World (MT Framework) — exe: MonsterHunterWorld.exe
## EXECUTION PRIORITY RANDOMIZATION


**Active Development Session:** Randomly selected target for parallel simulation execution


**Forced Random Execution Protocol:**
- [ ] System enforces immediate random target selection without user intervention
- [ ] Aggressive parallel simulation with forced context switching between engines
- [ ] Each forced session bypasses manual selection and jumps directly to implementation
- [ ] Automatic branch creation and forced commits for each random selection cycle
- [ ] Continuous random rotation prevents development stagnation on single engine
- [ ] Forced parallel streams maximize development velocity across all three adapters


**Current Random Selection:** 
```lua
math.randomseed(os.time())
local engines = {"RE-Engine", "REDengine4", "MT-Framework"}
local selected = engines[math.random(1, #engines)]
```


**Parallel Simulation Targets:**
- Session A: Resident Evil 7 (RE Engine) adapter development
- Session B: Cyberpunk 2077 (REDengine 4) adapter development  
- Session C: Monster Hunter: World (MT Framework) adapter development




## IMMEDIATE EXECUTION STATUS


**Cross-Engine VR Adapter Development (uevr Extension Framework):**


### Resident Evil 7 (RE Engine) - Cross-Engine Adapter: `adapters/RE-Engine/`
- [ ] RE Engine rendering pipeline hooks implemented via uevr's D3D11 abstraction
- [ ] Custom adapter plugin created extending uevr's cross-engine framework
- [ ] Stereo rendering compatibility layer built on uevr's rendering foundation
- [ ] Camera tracking integration adapted from uevr's view transformation system
- [ ] Motion controller support via uevr's input abstraction extended for RE Engine
- [ ] Performance optimization using uevr's frame timing and rendering optimizations
- [ ] Profile generation: `profiles/re7/adapter.json` with RE Engine-specific parameters


### Cyberpunk 2077 (REDengine 4) - Cross-Engine Adapter: `adapters/REDengine4/`
- [ ] REDengine 4 compatibility module developed using uevr's plugin architecture
- [ ] D3D12 rendering hooks established through uevr's graphics abstraction layer
- [ ] VR projection matrices calculated via uevr's stereo rendering mathematics
- [ ] Controller input mapping extended from uevr's motion controller framework
- [ ] UI overlay system adapted using uevr's interface projection capabilities
- [ ] Memory management protocols implemented following uevr's hooking safety patterns
- [ ] Cross-engine profile: `profiles/Cyberpunk2077/adapter.json` with REDengine parameters


### Monster Hunter: World (MT Framework) - Cross-Engine Adapter: `adapters/MT-Framework/`
- [ ] MT Framework rendering integration via uevr's cross-engine adapter foundation
- [ ] Third-person to first-person camera conversion using uevr's view transformation
- [ ] Animation system VR adaptation through uevr's skeletal tracking extensions
- [ ] Combat mechanics integration via uevr's motion controller interaction framework
- [ ] HUD and menu VR projection using uevr's UI rendering abstraction
- [ ] Performance profiling and optimization via uevr's engine-agnostic toolchain
- [ ] Adapter profile: `profiles/MonsterHunterWorld/adapter.json` with MT Framework config


**CRITICAL CROSS-ENGINE ADAPTER FILES:**
- `adapters/RE-Engine/engine_hooks.cpp` - RE Engine rendering pipeline integration
- `adapters/REDengine4/redengine_adapter.dll` - REDengine 4 compatibility layer
- `adapters/MT-Framework/mt_framework_bridge.cpp` - MT Framework VR integration
- `profiles/*/adapter.json` - Engine-specific VR parameters and configurations
- `docs/cross-engine/adapter_development.md` - Cross-engine implementation guide




## IMMEDIATE EXECUTION STATUS


**Cross-Engine VR Adapter Development (uevr Extension Framework):**


### Resident Evil 7 (RE Engine) - Cross-Engine Adapter: `adapters/RE-Engine/`
- [ ] RE Engine rendering pipeline hooks implemented via uevr's D3D11 abstraction
- [ ] Custom adapter plugin created extending uevr's cross-engine framework
- [ ] Stereo rendering compatibility layer built on uevr's rendering foundation
- [ ] Camera tracking integration adapted from uevr's view transformation system
- [ ] Motion controller support via uevr's input abstraction extended for RE Engine
- [ ] Performance optimization using uevr's frame timing and rendering optimizations
- [ ] Profile generation: `profiles/re7/adapter.json` with RE Engine-specific parameters


### Cyberpunk 2077 (REDengine 4) - Cross-Engine Adapter: `adapters/REDengine4/`
- [ ] REDengine 4 compatibility module developed using uevr's plugin architecture
- [ ] D3D12 rendering hooks established through uevr's graphics abstraction layer
- [ ] VR projection matrices calculated via uevr's stereo rendering mathematics
- [ ] Controller input mapping extended from uevr's motion controller framework
- [ ] UI overlay system adapted using uevr's interface projection capabilities
- [ ] Memory management protocols implemented following uevr's hooking safety patterns
- [ ] Cross-engine profile: `profiles/Cyberpunk2077/adapter.json` with REDengine parameters


### Monster Hunter: World (MT Framework) - Cross-Engine Adapter: `adapters/MT-Framework/`
- [ ] MT Framework rendering integration via uevr's cross-engine adapter foundation
- [ ] Third-person to first-person camera conversion using uevr's view transformation
- [ ] Animation system VR adaptation through uevr's skeletal tracking extensions
- [ ] Combat mechanics integration via uevr's motion controller interaction framework
- [ ] HUD and menu VR projection using uevr's UI rendering abstraction
- [ ] Performance profiling and optimization via uevr's engine-agnostic toolchain
- [ ] Adapter profile: `profiles/MonsterHunterWorld/adapter.json` with MT Framework config


**CRITICAL CROSS-ENGINE ADAPTER FILES:**
- `adapters/RE-Engine/engine_hooks.cpp` - RE Engine rendering pipeline integration
- `adapters/REDengine4/redengine_adapter.dll` - REDengine 4 compatibility layer
- `adapters/MT-Framework/mt_framework_bridge.cpp` - MT Framework VR integration
- `profiles/*/adapter.json` - Engine-specific VR parameters and configurations
- `docs/cross-engine/adapter_development.md` - Cross-engine implementation guide




# uevr Cross-Engine VR Adapter Development - Ongoing Session Prompt


## CRITICAL SESSION CONTINUATION
You are continuing cross-engine VR adapter development for EXACTLY these three games:
- **Resident Evil 7 (RE Engine)** — `G:\SteamLibrary\steamapps\common\RESIDENT EVIL 7 biohazard\re7.exe`
- **Cyberpunk 2077 (REDengine 4)** — `Cyberpunk2077.exe`
- **Monster Hunter: World (MT Framework)** — `MonsterHunterWorld.exe`


## MANDATORY REFERENCE DOCUMENTS
**Primary Implementation Guide:** `G:\GITHUB REPOs\uevr\implement.md`
- Execute EVERY section, EVERY line without deviation
- Follow exact workflow: compatibility → baseline → camera tuning → UObjectHook → validation


**Complete Documentation Sources:**
- `G:\GITHUB REPOs\uevr\docs\` - Full uevr documentation tree
- `G:\GITHUB REPOs\uevr\docs\uevr-docs\src\usage\overview.md`
- `G:\GITHUB REPOs\uevr\docs\uevr-docs\src\usage\vr_controller_bindings.md`
- `G:\GITHUB REPOs\uevr\docs\uevr-docs\src\usage\adding_6dof.md`
- `G:\GITHUB REPOs\uevr\docs\uevr-docs\src\plugins\lua\types\UObjectHook.md`
- `G:\GITHUB REPOs\uevr\SOURCECODE\` - Complete source analysis required


**Reference Implementation:** `G:\GITHUB REPOs\uevr\profiles\AbioticFactor-Win64-Shipping\`


## CURRENT STATUS CHECKPOINT
- [ ] RE7 adapter: Engine hooks identified and implemented
- [ ] CP2077 adapter: REDengine integration layer created
- [ ] MHW adapter: MT Framework compatibility module developed
- [ ] Cross-engine memory management protocols established
- [ ] Performance optimization parameters tuned per engine
- [ ] UI projection systems adapted for each engine
- [ ] Motion controller integration calibrated
- [ ] Comprehensive testing completed across all three games


## EXECUTION PROTOCOL
1. **Context7** for authoritative technical documentation (OpenXR, D3D11/12, hooking patterns)
2. **Deepwiki** for repository-specific guidance (REFramework, MinHook, RenderDoc workflows)
3. Cite ALL sources inline during development
4. Document progress in `profiles/<GameExeName>/README.md`
5. Maintain artifacts structure per implement.md specification


## FORBIDDEN ACTIONS
- Adding games beyond the three specified
- Skipping validation steps from implement.md
- Deviating from established folder structure in profiles/
- Omitting source citations from Context7/Deepwiki


**RESUME FROM:** Last completed checkpoint above and continue systematic execution.



1) Read and fully implement everything from:
   - g:\GITHUB REPOs\uevr\implement.md (EVERY Sections EVERY LINE: execution workflow, artifacts, validation, etc...)
   - DO NOT DEVIATE. No other games. No skipped steps.


2) Use docs tooling on EVERY step:  
   - Context7 for authoritative docs (OpenXR, D3D11/12, hooking, engine docs) and cite sources inline.
   - Deepwiki for repository‑specific guidance (REFramework, MinHook, RenderDoc workflows), cite inline.


3) Immediate Execution Protocol:
   - Identify the first unchecked item under CURRENT STATUS VERIFICATION.
   - Execute that item using implement.md Sections EVERY LINE.
   - Mark it complete, proceed to the next.
   - Document all changes immediately.


4) Mandatory Requirements:
   - Use exact executable names for any per‑game folder names.
   - After any state/attachment‑like adjustment, save persistent state (mirror the “Save State” discipline).
   - Weapon → right controller, utility → left controller.
   - Document all attachment/override paths in per‑game README.
   - Maintain strict artifacts structure from implement.md.


5) Forbidden:
   - Restarting from scratch
   - Skipping validation
   - Omitting attachment/override documentation
   - Bypassing performance/comfort testing
   - Deviating from folder structure
   - Modifying existing uevr source code files directly
   - Altering current file logic that breaks existing functionality
   - Changing core plugin architecture without proper extension points
   - Overwriting existing profiles without backup
   - Bypassing established hook mechanisms
   - Creating incompatible configuration formats
   - Breaking existing adapter compatibility


6) Artifacts to update/create per game (cross‑engine adapters):
   - docs/cross-engine/<Engine>.md (arch + hook points)
   - docs/cross-engine/<Game>.md (game notes, captures, CB/matrix findings)
   - profiles/<GameExeName>/README.md (exec name, config, attachments/overrides, validation, known quirks)
   - profiles/<GameExeName>/adapter.json (persistent adapter state: camera offsets, FOV policy, input mapping)
   - captures/renderdoc/*.rdc (evidence for matrices and main scene)


7) Report:
   - Post a compact status update with: what was executed, files touched, citations (Context7/Deepwiki), and next unchecked item.
   - Generate comprehensive reports to the G:\GITHUB REPOs\uevr\docs\reports folder documenting:
     * Profile creation workflow and validation results
     * UObjectHook attachment paths and controller mappings
     * Performance metrics and comfort settings analysis
     * Cross-engine adapter implementation details
     * Game-specific quirks and compatibility notesPERFORM NOW — Cross‑engine adapters for CP2077 / MHW / RE7


You are continuing cross‑engine VR adapter development for EXACTLY these three games only:
- Resident Evil 7 (RE Engine) — exe path: G:\SteamLibrary\steamapps\common\RESIDENT EVIL 7 biohazard\re7.exe
- Cyberpunk 2077 (REDengine 4) — exe: Cyberpunk2077.exe
- Monster Hunter: World (MT Framework) — exe: MonsterHunterWorld.exe


# uevr Cross-Engine VR Adapter Development - Ongoing Session Prompt


## CRITICAL SESSION CONTINUATION
You are continuing cross-engine VR adapter development for EXACTLY these three games:
- **Resident Evil 7 (RE Engine)** — `G:\SteamLibrary\steamapps\common\RESIDENT EVIL 7 biohazard\re7.exe`
- **Cyberpunk 2077 (REDengine 4)** — `Cyberpunk2077.exe`
- **Monster Hunter: World (MT Framework)** — `MonsterHunterWorld.exe`


## MANDATORY REFERENCE DOCUMENTS
**Primary Implementation Guide:** `G:\GITHUB REPOs\uevr\implement.md`
- Execute EVERY section, EVERY line without deviation
- Follow exact workflow: compatibility → baseline → camera tuning → UObjectHook → validation


**Complete Documentation Sources:**
- `G:\GITHUB REPOs\uevr\docs\` - Full uevr documentation tree
- `G:\GITHUB REPOs\uevr\docs\uevr-docs\src\usage\overview.md`
- `G:\GITHUB REPOs\uevr\docs\uevr-docs\src\usage\vr_controller_bindings.md`
- `G:\GITHUB REPOs\uevr\docs\uevr-docs\src\usage\adding_6dof.md`
- `G:\GITHUB REPOs\uevr\docs\uevr-docs\src\plugins\lua\types\UObjectHook.md`
- `G:\GITHUB REPOs\uevr\SOURCECODE\` - Complete source analysis required


**Reference Implementation:** `G:\GITHUB REPOs\uevr\profiles\AbioticFactor-Win64-Shipping\`


## CURRENT STATUS CHECKPOINT
- [ ] RE7 adapter: Engine hooks identified and implemented
- [ ] CP2077 adapter: REDengine integration layer created
- [ ] MHW adapter: MT Framework compatibility module developed
- [ ] Cross-engine memory management protocols established
- [ ] Performance optimization parameters tuned per engine
- [ ] UI projection systems adapted for each engine
- [ ] Motion controller integration calibrated
- [ ] Comprehensive testing completed across all three games


## EXECUTION PROTOCOL
1. **Context7** for authoritative technical documentation (OpenXR, D3D11/12, hooking patterns)
2. **Deepwiki** for repository-specific guidance (REFramework, MinHook, RenderDoc workflows)
3. Cite ALL sources inline during development
4. Document progress in `profiles/<GameExeName>/README.md`
5. Maintain artifacts structure per implement.md specification


## FORBIDDEN ACTIONS
- Adding games beyond the three specified
- Skipping validation steps from implement.md
- Deviating from established folder structure in profiles/
- Omitting source citations from Context7/Deepwiki


**RESUME FROM:** Last completed checkpoint above and continue systematic execution.



1) Read and fully implement everything from:
   - g:\GITHUB REPOs\uevr\implement.md (EVERY Sections EVERY LINE: execution workflow, artifacts, validation, etc...)
   - DO NOT DEVIATE. No other games. No skipped steps.


2) Use docs tooling on EVERY step:  
   - Context7 for authoritative docs (OpenXR, D3D11/12, hooking, engine docs) and cite sources inline.
   - Deepwiki for repository‑specific guidance (REFramework, MinHook, RenderDoc workflows), cite inline.


3) Immediate Execution Protocol:
   - Identify the first unchecked item under CURRENT STATUS VERIFICATION.
   - Execute that item using implement.md Sections EVERY LINE.
   - Mark it complete, proceed to the next.
   - Document all changes immediately.


4) Mandatory Requirements:
   - Use exact executable names for any per‑game folder names.
   - After any state/attachment‑like adjustment, save persistent state (mirror the “Save State” discipline).
   - Weapon → right controller, utility → left controller.
   - Document all attachment/override paths in per‑game README.
   - Maintain strict artifacts structure from implement.md.


5) Forbidden:
   - Restarting from scratch
   - Skipping validation
   - Omitting attachment/override documentation
   - Bypassing performance/comfort testing
   - Deviating from folder structure
   - Modifying existing uevr source code files directly
   - Altering current file logic that breaks existing functionality
   - Changing core plugin architecture without proper extension points
   - Overwriting existing profiles without backup
   - Bypassing established hook mechanisms
   - Creating incompatible configuration formats
   - Breaking existing adapter compatibility


6) Artifacts to update/create per game (cross‑engine adapters):
   - docs/cross-engine/<Engine>.md (arch + hook points)
   - docs/cross-engine/<Game>.md (game notes, captures, CB/matrix findings)
   - profiles/<GameExeName>/README.md (exec name, config, attachments/overrides, validation, known quirks)
   - profiles/<GameExeName>/adapter.json (persistent adapter state: camera offsets, FOV policy, input mapping)
   - captures/renderdoc/*.rdc (evidence for matrices and main scene)


7) Report:
   - Post a compact status update with: what was executed, files touched, citations (Context7/Deepwiki), and next unchecked item.
   - Generate comprehensive reports to the G:\GITHUB REPOs\uevr\docs\reports folder documenting:
     * Profile creation workflow and validation results
     * UObjectHook attachment paths and controller mappings
     * Performance metrics and comfort settings analysis
     * Cross-engine adapter implementation details
     * Game-specific quirks and compatibility notes