@echo off
setlocal
set SCRIPT=%~dp0build_framework.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File "%SCRIPT%" %*
set RC=%ERRORLEVEL%
exit /b %RC%
