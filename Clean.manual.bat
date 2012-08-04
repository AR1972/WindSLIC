@ECHO OFF
DEL /Q /F "%~dp0WindSLIC.sdf"
RMDIR /Q /S "%~dp0Release"
RMDIR /Q /S "%~dp0Debug"
RMDIR /Q /S "%~dp0ipch"

RMDIR /Q /S "%~dp0WindSLIC_EFI\X64"
DEL /Q /F "%~dp0WindSLIC_EFI\BOOTX64.EFI"

RMDIR /Q /S "%~dp0libinstaller\Release"
RMDIR /Q /S "%~dp0libinstaller\Debug"
DEL /Q /F "%~dp0libinstaller\libinstaller.lib"

RMDIR /Q /S "%~dp0Installer_NTFS_cli\Release"
RMDIR /Q /S "%~dp0Installer_NTFS_cli\Debug"
DEL /Q /F "%~dp0Installer_NTFS_cli\WINDSLIC.BIN"

RMDIR /Q /S "%~dp0Installer_EFI_gui\Release"
RMDIR /Q /S "%~dp0Installer_EFI_gui\Debug"

RMDIR /Q /S "%~dp0Installer_EFI_cli\Release"
RMDIR /Q /S "%~dp0Installer_EFI_cli\Debug"

RMDIR /Q /S "%~dp0Installer_bootmgr_gui\obj"

DEL /Q /F "%~dp0Installer_bootmgr_gui\Resources\bootmgr.gz"