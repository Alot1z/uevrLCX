---
description: Auto-start never-stop forskningsloop (template til .windsurf) med anbefalede defaults
---

Formål
- Start den uendelige research-cyklus med "bedste" standarder automatisk.

Forudsætninger
- Windows + PowerShell
- Python 3.10+ på PATH

1) Opret venv og installer afhængigheder (første gang)
```powershell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
python -m pip install --upgrade pip
if (Test-Path requirements.txt) { pip install -r requirements.txt }
```

// turbo
2) Sæt anbefalede defaults (session)
```powershell
$env:UEVR_AUTOCOMMIT = '1'
$env:UEVR_MAX_RETRIES = '8'
$env:UEVR_BASE_DELAY  = '5'
$env:UEVR_SUCCESS_IDLE= '600'
$env:UEVR_FAIL_IDLE   = '300'
```

// turbo
3) Start never-stop loop
```powershell
powershell -ExecutionPolicy Bypass -File .windsurf/scripts/research_loop.ps1
```

Stop
- Luk terminalen for at stoppe loopet.
