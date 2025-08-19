# Build Prompt — Monster Hunter: World (MT Framework)

Goal: zero-warning Release build of MT-Framework adapter; strict logs and summary.

## Run
- PowerShell: `tools/build_adapters/MT-Framework/build_mhw.ps1 -Config Release`
- Batch: `tools/build_adapters/MT-Framework/build_mhw.bat`

## Behavior
- CMake VS 2022 (if available), x64
- Flags: `/W4 /WX /MP` (warnings as errors)
- Logs: `logs/build/MT-Framework/<timestamp>/{configure.log, build.log, BuildSummary.md}`
- Deploys `mt_framework_cross_engine_adapter.dll` to `profiles/MonsterHunterWorld/`
- Fails build if any warnings or errors are detected in logs

## Troubleshooting
- Use `-Clean` to force reconfigure
- MT-Framework CMake already enforces `/WX` for Release

## Pasteback — Build Results
- Date/Hash: [YYYY-MM-DD] / [commit]
- Outcome: [pass/fail]
- Warnings: [0]
- Errors: [0]
- Generator: [VS 2022/Default]
- Artifacts: [paths]
- Logs: [links to logs dir]

---
