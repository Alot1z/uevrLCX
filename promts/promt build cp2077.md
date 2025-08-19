# Build Prompt — Cyberpunk 2077 (REDengine 4)

Goal: zero-warning Release build of REDengine4 adapter; strict logs and summary.

## Run
- PowerShell: `tools/build_adapters/REDengine4/build_cp2077.ps1 -Config Release`
- Batch: `tools/build_adapters/REDengine4/build_cp2077.bat`

## Behavior
- CMake VS 2022 (if available), x64
- Flags: `/W4 /WX /MP` (warnings as errors)
- Logs: `logs/build/REDengine4/<timestamp>/{configure.log, build.log, BuildSummary.md}`
- Deploys `redengine4_adapter.dll` to `profiles/Cyberpunk2077/`
- Fails build if any warnings or errors are detected in logs

## Troubleshooting
- Use `-Clean` to force reconfigure
- Note: output may be under build/Release or build/bin; script auto-detects

## Pasteback — Build Results
- Date/Hash: [YYYY-MM-DD] / [commit]
- Outcome: [pass/fail]
- Warnings: [0]
- Errors: [0]
- Generator: [VS 2022/Default]
- Artifacts: [paths]
- Logs: [links to logs dir]

---
