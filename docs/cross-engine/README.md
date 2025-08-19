# Cross-Engine VR Adapter Documentation

Scope: Resident Evil 7 (RE Engine), Cyberpunk 2077 (REDengine 4), Monster Hunter: World (MT Framework)

Directory structure:
- `RE-Engine.md` — engine-level strategy for RE Engine
- `REDengine4.md` — engine-level strategy for REDengine 4
- `MT-Framework.md` — engine-level strategy for MT Framework
- `ResidentEvil7.md` — per-game findings, captures, matrices, attachments
- `Cyberpunk2077.md` — per-game findings, captures, matrices, attachments
- `MonsterHunterWorld.md` — per-game findings, captures, matrices, attachments
- `adapter_json_schema.md` — canonical adapter.json schema used in `profiles/<Game>/adapter.json`
- `capture-template.md` — what to extract from RenderDoc captures

How to use:
1) Start with engine page to outline integration + render pipeline hooks.
2) For each game, fill the per-game page using the capture template.
3) Keep `profiles/<Game>/adapter.json` in sync with these docs.
4) Cite sources via Context7/Deepwiki inline where relevant.
