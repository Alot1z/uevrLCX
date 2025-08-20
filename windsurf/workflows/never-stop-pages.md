---
description: Never-stop local Pages build (template for .windsurf) — Context7 + DeepWiki + frontend
---

Formål
- Lokal nonstop build-loop for frontend, der også regenererer Context7 og DeepWiki mellem builds.

Forudsætninger
- Windows + PowerShell
- Python 3.10+ og Node.js 18+ på PATH

1) Opsæt venv og installer afhængigheder (første gang)
```powershell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
python -m pip install --upgrade pip
if (Test-Path requirements.txt) { pip install -r requirements.txt }
```

2) Installer frontend-deps (i docs/github-pages/)
```powershell
pushd docs/github-pages
npm ci
popd
```

// turbo
3) Start never-stop loop (lokal build)
```powershell
$ErrorActionPreference = 'Stop'
$maxRetries = 5
$baseDelay = 10

while ($true) {
  $attempt = 0
  $ok = $false
  while (-not $ok -and $attempt -lt $maxRetries) {
    try {
      .\.venv\Scripts\Activate.ps1
      # Context7
      python scripts/generate_context7_database.py
      # DeepWiki
      powershell -ExecutionPolicy Bypass -File tools/10_CREATE_FINAL_DEEPWIKI.ps1
      # Frontend build
      pushd docs/github-pages
      npm run build
      popd
      $ok = $true
    } catch {
      $attempt++
      $delay = [Math]::Min($baseDelay * [Math]::Pow(2, $attempt), 300)
      Write-Warning "[WARN] Build failed (attempt $attempt/$maxRetries): $_"
      Start-Sleep -Seconds $delay
    }
  }
  if (-not $ok) { Start-Sleep -Seconds 300 } else { Start-Sleep -Seconds 900 }
}
```

Noter
- Stop loopet ved at lukke terminalen.
- Denne skabelon er tiltænkt at blive kopieret til `.windsurf/workflows/` når du er tilfreds.
