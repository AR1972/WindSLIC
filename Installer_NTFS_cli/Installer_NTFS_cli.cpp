#include "stdafx.h"
#include <VersionHelpers.h>
#include <new>
using namespace std;
//
typedef NTSTATUS (WINAPI *QuerySystemInformation)(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength);

static QuerySystemInformation pNtQuerySystemInformation = NULL;
static HMODULE hNtdll = NULL;
wchar_t* eloadresource = L"load resource failed";

BOOL
	InitLib(BOOL Load)
{
	BOOL RetVal = FALSE;
	if(Load) {
		if (!hNtdll) {
			hNtdll = LoadLibrary(L"ntdll.dll");
		}
		if(hNtdll) {
			if (!pNtQuerySystemInformation) {
				pNtQuerySystemInformation = (QuerySystemInformation) GetProcAddress(hNtdll, "NtQuerySystemInformation");
			}
		}
		RetVal = (pNtQuerySystemInformation != NULL);
	} else {
		if (hNtdll) {
			pNtQuerySystemInformation = NULL;
			RetVal = FreeLibrary(hNtdll);
			hNtdll = NULL;
		}
	}
	return RetVal;
}

BOOL
isEfi(VOID)
{
	BOOL RetVal = FALSE;
	DWORD dwPInfo = NULL;
	DWORD dwVersion = NULL;
	DWORD dwMajorVersion = NULL;
	DWORD dwMinorVersion = NULL;
	dwVersion = GetVersion();
	dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
	if (dwMajorVersion == 6 || dwMinorVersion == 1) {
		if (!InitLib(TRUE)) {
			return RetVal;
		}
		DWORD buffer[5] = {};
		if (pNtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)90, buffer, sizeof(buffer), NULL) == 0 && buffer[4] == 2) {
			RetVal = TRUE;
		}
	}
	else if (IsWindows8OrGreater()) {
		FIRMWARE_TYPE FirmwareType;
		GetFirmwareType(&FirmwareType);
		RetVal = FirmwareType == FirmwareTypeUefi;
	}
	return RetVal;
}

int _tmain(int argc, _TCHAR* argv[])
{
	ULONG Status = NULL;
	UCHAR* Buffer = NULL;
	wchar_t* VolumeName = NULL;
	wchar_t* DriveName = NULL;
	HANDLE hDevice = NULL;
	ULONG BufferLength = 128;
	DWORD BytesRead = NULL;
	DWORD BytesWritten = NULL;
	DWORD BytesReturned = NULL;
	DWORD LastError = NULL;
	HRSRC hResource = NULL;
	HGLOBAL hgResource = NULL;
	ULONG BootSectSize = NULL;
	UCHAR* BootSect = NULL;
	PVOLUME_DISK_EXTENTS pDiskExtents = NULL;
	PDRIVE_LAYOUT_INFORMATION_EX pDriveLayoutInfo = NULL;
	UCHAR Key[8] = {};
	ULONG SlicSize = NULL;
	UCHAR* Slic = NULL;
	UCHAR* Resource = NULL;
	BOOL Uninstall = FALSE;
	//
	if (!InitLib(TRUE)) {
		wprintf_s(L"lib initialization error\n");
		return 1;
	}
	if(isEfi()){
		wprintf_s(L"unsupported EFI system");
		return 2;
	}
	if((argc > 1) && (wcscmp(argv[1], L"/u") == 0)) {
		Uninstall = TRUE;
	}
	try {
		if(Uninstall) {
			hResource = FindResource(NULL, MAKEINTRESOURCE(103), L"RAW");
		}
		else {
			hResource = FindResource(NULL, MAKEINTRESOURCE(101), L"RAW");
		}
		if(hResource) {
			BootSectSize = SizeofResource(NULL, hResource);
			hgResource = LoadResource(NULL, hResource);
		}
		else {
			wprintf_s(L"%s", eloadresource);
			return 1;
		}
		if(hgResource) {
			Resource = (UCHAR*)LockResource(hgResource);
		}
		else {
			wprintf_s(L"%s", eloadresource);
			return 1;
		}
		BootSect = new UCHAR[BootSectSize];
		memcpy(BootSect, Resource, BootSectSize);
		// get slic resource.
		hResource = FindResource(NULL, MAKEINTRESOURCE(102), L"RAW");
		if(hResource) {
			SlicSize = SizeofResource(NULL, hResource);
			hgResource = LoadResource(NULL, hResource);
		}
		else {
			wprintf_s(L"%s", eloadresource);
			return 1;
		}
		if(hgResource) {
			Resource = (UCHAR*)LockResource(hgResource);
		}
		else {
			wprintf_s(L"%s", eloadresource);
			return 1;
		}
		Slic = new UCHAR[SlicSize];
		memcpy(Slic, Resource, SlicSize);
		// get system volume.
		VolumeName = new wchar_t[MAX_PATH];
		DriveName = new wchar_t[MAX_PATH];
		Buffer = new UCHAR[BufferLength]();
		pNtQuerySystemInformation((SYSTEM_INFORMATION_CLASS) 98, Buffer, BufferLength, &BufferLength);
		wsprintf(VolumeName, L"\\\\.\\%s", (wchar_t*)(Buffer + 24));
		hDevice = CreateFile(VolumeName, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		delete[] Buffer;
		// get partition data.
		Buffer = new UCHAR[BootSectSize]();
		Status = ReadFile(hDevice, Buffer, BootSectSize, &BytesRead, NULL);
		// check filesystem type.
		if(memcmp(Buffer + 3, "NTFS    \0", 9) != 0) {
			delete[] BootSect;
			delete[] Slic;
			delete[] VolumeName;
			delete[] DriveName;
			delete[] Buffer;
			wprintf_s(L"unsupported file system type");
			return 1;
		}
		memcpy(BootSect, Buffer, 0x54);
		delete[] Buffer;
		// get system disk.
		BufferLength = 16;
		Buffer = new UCHAR[BufferLength];
		memset(Buffer, 0, BufferLength);
		while (!DeviceIoControl(hDevice, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, NULL, Buffer, BufferLength, &BytesReturned, NULL)) {
			BufferLength += 16;
			delete[] Buffer;
			Buffer = new UCHAR[BufferLength];
			memset(Buffer, 0, BufferLength);
		}
		CloseHandle(hDevice);
		pDiskExtents = (PVOLUME_DISK_EXTENTS) Buffer;
		wsprintf(DriveName, L"\\\\.\\PhysicalDrive%d\0", pDiskExtents->Extents[0].DiskNumber);
		hDevice = CreateFile(DriveName, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		delete[] Buffer;
		// get system disk partition layout.
		BufferLength = 16;
		Buffer = new UCHAR[BufferLength];
		while (!DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, NULL, Buffer, BufferLength, &BytesReturned, NULL)) {
			BufferLength += 16;
			delete[] Buffer;
			Buffer = new UCHAR[BufferLength];
			memset(Buffer, 0, BufferLength);
		}
		CloseHandle(hDevice);
		pDriveLayoutInfo = (PDRIVE_LAYOUT_INFORMATION_EX) Buffer;
		if(pDriveLayoutInfo->PartitionStyle == PARTITION_STYLE_GPT) {
			delete[] BootSect;
			delete[] Slic;
			delete[] VolumeName;
			delete[] DriveName;
			delete[] Buffer;
			wprintf_s(L"unsupported partition type");
			return 1;
		}
		if(!Uninstall) {
			// prepare key/SLIC
			srand((UINT)time(0));
			for (UINT i = 0; i < sizeof(Key); i++) {
				Key[i] = (UCHAR)(rand() % 0x5D) + 0x21;
			}
			// encrypt slic.
			UINT pos = 0;
			for (UINT i = 0; i < SlicSize; i++) {
				Slic[i] = Slic[i] + 127 + Key[pos];
				pos++;
				if (pos == sizeof(Key)) {
					pos = 0;
				}
			}
			// encrypt key.
			for (UINT i = 0; i < sizeof(Key); i++) {
				Key[i] ^= 0xFF;
			}
			// copy encrypted data.
			for(UINT i = 0; i < BootSectSize-6; i++) {
				//
				if (BootSect[i] == 0x4B && BootSect[i+1] == 0x45
					&& BootSect[i+2] == 0x59 && BootSect[i+3] == 0x4B
					&& BootSect[i+4] == 0x45 && BootSect[i+5] == 0x59) {
						memcpy(&BootSect[i], Key, sizeof(Key));
				}
				//
				if (BootSect[i] == 0x53 &&BootSect[i+1] == 0x4C
					&& BootSect[i+2] == 0x49 && BootSect[i+3] == 0x43
					&& BootSect[i+4] == 0x76 && BootSect[i+5] == 0x01) {
						memcpy(&BootSect[i], Slic, SlicSize);
				}
			}
		}
		hDevice = CreateFile(VolumeName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING|FILE_FLAG_WRITE_THROUGH, NULL);
		DeviceIoControl(hDevice, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &BytesReturned, NULL);
		if(Uninstall) {
			wprintf_s(L"uninstalling WindSLIC: ");
		}
		else {
			wprintf_s(L"installing WindSLIC: ");
		}
		if(WriteFile(hDevice, BootSect, BootSectSize, &BytesWritten, NULL)) {
			wprintf_s(L"success\n");
		}
		else {
			wprintf_s(L"fail\n");
		}
		DeviceIoControl(hDevice, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &BytesReturned, NULL);
		CloseHandle(hDevice);
		// clean up
		delete[] BootSect;
		delete[] Slic;
		delete[] VolumeName;
		delete[] DriveName;
		delete[] Buffer;
		return 0;
	} catch (bad_alloc) {
		if(NULL != BootSect) {delete[] BootSect;}
		if(NULL != Slic) {delete[] Slic;}
		if(NULL != VolumeName) {delete[] VolumeName;}
		if(NULL != DriveName) {delete[] DriveName;}
		if(NULL != Buffer) {delete[] Buffer;}
		return 1;
	}
}