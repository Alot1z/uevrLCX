@echo off
setlocal
set SCRIPT=%~dp0build_cp2077.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File "%SCRIPT%" %*
set RC=%ERRORLEVEL%
exit /b %RC%
