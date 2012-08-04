@if "%1" neq "" (set CONFIG=%1) else (set CONFIG=Release)
call Build.bat %CONFIG% x64 WindSLIC_EFI %2
call :color a "All builds succeeded"
REM Functions
exit /b
:color
@echo off
setlocal
pushd %temp%
for /F "tokens=1 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (
  <nul set/p"=%%a" >"%~2"
)
findstr /v /a:%1 /R "^$" "%~2" nul
del "%~2" > nul 2>&1
popd
@echo on
exit /b