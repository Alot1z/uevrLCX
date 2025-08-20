---
description: Never-stop research loop (template for .windsurf) — Context7 + AI + Compatibility Matrix + DeepWiki
---

Formål
- Kør en uendelig, robust cyklus lokalt i Windsurf (ikke GitHub):
  - Regenerér Context7
  - Kør AI-strukturanalyse
  - Generér Compatibility Matrix (MD + JSON)
  - Regenerér DeepWiki
  - Valgfrit auto-commit/push

Forudsætninger
- Windows + PowerShell
- Python 3.10+ på PATH
- Git konfigureret (hvis auto-commit/push ønskes)

1) Opret venv og installer afhængigheder (kun første gang)
```powershell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
python -m pip install --upgrade pip
if (Test-Path requirements.txt) { pip install -r requirements.txt }
```

2) (Valgfrit) Sæt anbefalede defaults for nonstop drift
```powershell
$env:UEVR_AUTOCOMMIT = '1'
$env:UEVR_MAX_RETRIES = '8'
$env:UEVR_BASE_DELAY  = '5'
$env:UEVR_SUCCESS_IDLE= '600'
$env:UEVR_FAIL_IDLE   = '300'
```

// turbo
3) Start uendeligt loop (via .windsurf script)
```powershell
powershell -ExecutionPolicy Bypass -File .windsurf/scripts/research_loop.ps1
```

Bemærkninger
- Scriptet `.windsurf/scripts/research_loop.ps1` er robust til arbejdsmappen (finder repo-roden automatisk).
- Fejl håndteres med eksponentiel backoff og retrier; cyklussen fortsætter.
- Med `UEVR_AUTOCOMMIT=1` pushes ændringer automatisk.
