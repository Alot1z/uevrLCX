# Build Prompt — CrossEngine Main Framework

Goal: zero-warning Release build of core framework; strict logs and summary.

## Run
- PowerShell: `tools/build_adapters/CrossEngine/build_framework.ps1 -Config Release`
- Batch: `tools/build_adapters/CrossEngine/build_framework.bat`

## Behavior
- CMake VS 2022 (if available), x64
- Flags: `/W4 /WX /MP` (warnings as errors)
- Logs: `logs/build/CrossEngine/<timestamp>/{configure.log, build.log, BuildSummary.md}`
- Fails build if any warnings or errors are detected in logs

## Troubleshooting
- Use `-Clean` to force reconfigure
- Check `SOURCECODE/uevr/CMakeLists.txt` for warning policy

## Pasteback — Build Results
- Date/Hash: [YYYY-MM-DD] / [commit]
- Outcome: [pass/fail]
- Warnings: [0]
- Errors: [0]
- Generator: [VS 2022/Default]
- Artifacts: [paths]
- Logs: [links to logs dir]

---
