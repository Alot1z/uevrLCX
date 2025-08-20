---
description: Windsurf regler (template) for ALDRIG-STOP auto-restart og robust drift
---

Mål
- Cascade/Windsurf må ikke stoppe; altid genstarte og fortsætte loopet.
- Context7 ⇄ DeepWiki beriger hinanden hvert gennemløb (inkl. AI & Matrix).

Regler
- Aldrig-stop: ved fejl → retry med eksponentiel backoff; efter max-forsøg → hvil og fortsæt.
- Sikkerhed: ingen sletninger/eksterne kald uden eksplicit tilladelse.
- Observabilitet: log hvert forsøg og næste ventetid.

Anbefalede defaults
- `UEVR_AUTOCOMMIT=1`
- `UEVR_MAX_RETRIES=8`
- `UEVR_BASE_DELAY=5`
- `UEVR_SUCCESS_IDLE=600`
- `UEVR_FAIL_IDLE=300`

// turbo
Sæt defaults (kan køres i sessionen)
```powershell
$env:UEVR_AUTOCOMMIT = '1'
$env:UEVR_MAX_RETRIES = '8'
$env:UEVR_BASE_DELAY  = '5'
$env:UEVR_SUCCESS_IDLE= '600'
$env:UEVR_FAIL_IDLE   = '300'
```

Relaterede workflows
- `windsurf/workflows/never-stop-research.md`
- `windsurf/workflows/auto-start-never-stop.md`
