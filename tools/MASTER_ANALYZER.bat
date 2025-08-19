@echo off
echo ========================================
echo UEVR MASTER ANALYZER - KOORDINERER ALT
echo ========================================
echo.

echo Starter PowerShell scripts for at analysere alle filer...
echo.

echo [1/10] Analyserer core filer...
powershell -ExecutionPolicy Bypass -File "01_ANALYZE_CORE.ps1"

echo [2/10] Analyserer hooks filer...
powershell -ExecutionPolicy Bypass -File "02_ANALYZE_HOOKS.ps1"

echo [3/10] Analyserer mods filer...
powershell -ExecutionPolicy Bypass -File "03_ANALYZE_MODS.ps1"

echo [4/10] Analyserer adapters filer...
powershell -ExecutionPolicy Bypass -File "04_ANALYZE_ADAPTERS.ps1"

echo [5/10] Analyserer VR filer...
powershell -ExecutionPolicy Bypass -File "05_ANALYZE_VR.ps1"

echo [6/10] Analyserer ImGui filer...
powershell -ExecutionPolicy Bypass -File "06_ANALYZE_IMGUI.ps1"

echo [7/10] Analyserer utility filer...
powershell -ExecutionPolicy Bypass -File "07_ANALYZE_UTILITY.ps1"

echo [8/10] Analyserer manglende funktioner...
powershell -ExecutionPolicy Bypass -File "08_ANALYZE_MISSING_FUNCTIONS.ps1"

echo [9/10] Samler alle analyser...
powershell -ExecutionPolicy Bypass -File "09_MERGE_ALL_ANALYSES.ps1"

echo [10/10] Opretter komplet deepwiki...
powershell -ExecutionPolicy Bypass -File "10_CREATE_FINAL_DEEPWIKI.ps1"

echo.
echo ========================================
echo ALLE ANALYSER FÆRDIGE!
echo ========================================
echo.
echo Filer oprettet i docs\deepwiki\:
echo - CORE_ANALYSE.md
echo - HOOKS_ANALYSE.md
echo - MODS_ANALYSE.md
echo - ADAPTERS_ANALYSE.md
echo - VR_ANALYSE.md
echo - IMGUI_ANALYSE.md
echo - UTILITY_ANALYSE.md
echo - MANGLENDE_FUNKTIONER.md
echo - ALLE_ANALYSER_SAMLET.md
echo - DEEPWIKI_KOMPLET.md
echo.
echo ========================================
echo DEEPWIKI SYSTEM OPRETTET!
echo ========================================
echo Du har nu den dybe tallerken for alle projekters fulde filer!
echo Alle filer gemmes i docs\deepwiki\ mappen!
echo.
pause
