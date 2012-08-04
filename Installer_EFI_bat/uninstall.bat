@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION
SET BCDEDIT=%SYSTEMROOT%\System32\bcdedit.exe
SET FREEDRIVELETTER=0
SET /a COUNT=0
::
ECHO removing boot entry.
ECHO please wait...
%BCDEDIT% /set {bootmgr} path "\EFI\Microsoft\Boot\bootmgfw.efi" >nul
FOR /F "tokens=2" %%A IN ('%BCDEDIT% /enum BOOTMGR ^| FINDSTR /I /R /C:"{........-.*}"') DO (
	%BCDEDIT% /enum %%A | FIND /I "\EFI\WindSLIC\BOOTX64.EFI" >nul
	IF NOT !ERRORLEVEL!==1 (
		SET /A COUNT=%COUNT%+1
		ECHO found WindSLIC boot entry.
		ECHO deleting %%A
		%BCDEDIT% /delete %%A >nul
		ECHO setting boot order.
		%BCDEDIT% /set {fwbootmgr} displayorder {bootmgr} /addfirst >nul
	)
)
IF %COUNT%==0 ECHO WindSLIC boot entry not found.
"%~dp0Installer_EFI_cli.exe" /u
PAUSE