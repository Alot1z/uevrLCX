# Build Prompt — Resident Evil 7 (RE Engine)

Goal: zero-warning Release build of RE7 adapter; strict logs and summary.

## Run
- PowerShell: `tools/build_adapters/RE-Engine/build_re7.ps1 -Config Release`
- Batch: `tools/build_adapters/RE-Engine/build_re7.bat`

## Behavior
- CMake VS 2022 (if available), x64
- Flags: `/W4 /WX /MP` (warnings as errors)
- Logs: `logs/build/RE-Engine/<timestamp>/{configure.log, build.log, BuildSummary.md}`
- Deploys `RE_Engine_Adapter.dll` to `profiles/re7/`
- Fails build if any warnings or errors are detected in logs

## Troubleshooting
- Use `-Clean` to force reconfigure
- Check `adapters/RE-Engine/CMakeLists.txt` for warning policy

## Pasteback — Build Results
- Date/Hash: [YYYY-MM-DD] / [commit]
- Outcome: [pass/fail]
- Warnings: [0]
- Errors: [0]
- Generator: [VS 2022/Default]
- Artifacts: [paths]
- Logs: [links to logs dir]

---
