//
/* This program is free software. It comes without any warranty, to
* the extent permitted by applicable law. You can redistribute it
* and/or modify it under the terms of the Do What The Fuck You Want
* To Public License, Version 2, as published by Sam Hocevar. See
* http://sam.zoy.org/wtfpl/COPYING for more details. */
//
#include "stdafx.h"
#include "libinstaller.h"
#include <VersionHelpers.h>
#include <new>
using namespace std;
//
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
		if (IsWindows8OrGreater()) {
			if (!hKernel32) {
				hKernel32 = LoadLibrary(L"Kernel32.dll");
			}
			if (hKernel32) {
				if (!pGetFirmwareType) {
					pGetFirmwareType = (nGetFirmwareType)GetProcAddress(hKernel32, "GetFirmwareType");
				}
			}
			RetVal = (pNtQuerySystemInformation != NULL) & (pGetFirmwareType != NULL);
		}
	} else {
		if (hNtdll) {
			pNtQuerySystemInformation = NULL;
			FreeLibrary(hNtdll);
			hNtdll = NULL;
		}
		if (hKernel32) {
			pGetFirmwareType = NULL;
			FreeLibrary(hKernel32);
			hKernel32 = NULL;
		}
		RetVal = (pNtQuerySystemInformation == NULL) & (pGetFirmwareType == NULL);
	}
	return RetVal;
}
//
BOOL
	Test(VOID)
{
	UINT InBootOrderLength = 1;
	USHORT* InBootOrder = new USHORT[InBootOrderLength];
	BOOL RetVal = TRUE;
	UINT RetLen = 0;
	//
	RetLen = GetFirmwareEnvironmentVariable(TEXT("BootOrder"), EfiGuid, InBootOrder, (InBootOrderLength * sizeof(USHORT)));
	if (GetLastError() == ERROR_INVALID_FUNCTION) {
		RetVal = FALSE;
	}
	delete[] InBootOrder;
	return RetVal;
}
//
int
	UniqueId(wchar_t** Id)
{
	UCHAR* Buffer = NULL;
	wchar_t* UniqueId = NULL;
	wchar_t* DriveName = NULL;
	wchar_t* VolumeName = NULL;
	UCHAR PartitionId[sizeof(GUID)] = {};
	UCHAR PartitionType[sizeof(GUID)] = {};
	wchar_t tmp[4] = {};
	ULONG BytesReturned;
	HANDLE hDevice;
	PVOLUME_DISK_EXTENTS pDiskExtents;
	PDRIVE_LAYOUT_INFORMATION_EX pDriveLayoutInfo;
	ULONG BufferLength = 128;
	//
	try {
		if (!InitLib(TRUE)) {
			*Id = NULL;
			return ERROR_INVALID_HANDLE;
		}
		Buffer = new UCHAR[BufferLength];
		UniqueId = new wchar_t[sizeof(GUID) * sizeof(wchar_t) + 4];
		memset(UniqueId, 0, sizeof(GUID) * sizeof(wchar_t) + 4);
		DriveName = new wchar_t[MAX_PATH];
		VolumeName = new wchar_t[MAX_PATH];
		memset(Buffer, 0, BufferLength);
		// get system volume.
		pNtQuerySystemInformation((SYSTEM_INFORMATION_CLASS) 98, Buffer, BufferLength, &BufferLength);
		wsprintf(VolumeName, L"\\\\.\\%s\0", (wchar_t*)(Buffer + 24));
		hDevice = CreateFile(VolumeName, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
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
		for (UINT i = 0; i < pDriveLayoutInfo->PartitionCount; i++) {
			if (pDriveLayoutInfo->PartitionStyle == PARTITION_STYLE_GPT) {
				if (memcmp(&pDriveLayoutInfo->PartitionEntry[i].Gpt.PartitionType, &ESP, sizeof(GUID)) == 0) {
					memcpy(PartitionId, &pDriveLayoutInfo->PartitionEntry[i].Gpt.PartitionId, sizeof(GUID));
					memcpy(PartitionType, &pDriveLayoutInfo->PartitionEntry[i].Gpt.PartitionType, sizeof(GUID));
					for (UINT j = 0; j < sizeof(GUID); j++) {
						PartitionId[j] ^= PartitionType[j];
					}
					for (UINT j = 0; j < sizeof(GUID)/2; j++) {
						wsprintf(tmp, L"%02x", PartitionId[j]);
						wcscat_s(*Id, sizeof(GUID) * 2 + 4, tmp);
					}
				}
			}
		}
		delete[] DriveName;
		delete[] VolumeName;
		delete[] Buffer;
		return ERROR_SUCCESS;
	} 
	catch (bad_alloc) {
		if(NULL != DriveName) {delete[] DriveName;}
		if(NULL != VolumeName) {delete[] VolumeName;}
		if(NULL != Buffer) {delete[] Buffer;}
		*Id = NULL;
		return ERROR_OUTOFMEMORY;
	}
}
//
UINT
	DeleteRecursive(wchar_t* Directory)
{
	HANDLE hFind = NULL;
	WIN32_FIND_DATA FindFileData;
	static UINT RetVal = 0;
	wchar_t* File = NULL;
	wchar_t* Recurse = NULL;
	wchar_t* Find = NULL;
	try {
		File = new wchar_t[MAX_PATH];
		Recurse = new wchar_t[MAX_PATH];
		Find = new wchar_t[MAX_PATH];
		wsprintf(Find, L"%s\\*.*\0", Directory);
		//
		hFind = FindFirstFile(Find, &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE) {
			delete[] Find;
			delete[] File;
			delete[] Recurse;
			return 0;
		}
		do
		{
			if (wcscmp(FindFileData.cFileName, L".") && wcscmp(FindFileData.cFileName, L"..") != 0 ) {
				if (FindFileData.dwFileAttributes & ~FILE_ATTRIBUTE_DIRECTORY) {
					wsprintf(File, L"%s\\%s\0" , Directory, FindFileData.cFileName);
					if (DeleteFile(File)) {
						RetVal++;
					}
				}
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					wsprintf(Recurse, L"%s\\%s\0", Directory, FindFileData.cFileName);
					DeleteRecursive(Recurse);
				}
			}
		} while(FindNextFile(hFind, &FindFileData));
		if (RemoveDirectory(Directory)) {
			RetVal++;
		}
		FindClose(hFind);
		delete[] Find;
		delete[] File;
		delete[] Recurse;
		return RetVal;
	}
	catch (bad_alloc) {
		if(NULL != Find) {delete[] Find;}
		if(NULL != File) {delete[] File;}
		if(NULL != Recurse) {delete[] Recurse;}
		return ERROR_OUTOFMEMORY;
	}
}
//
BOOL
	EfiBootOrderAddFirst(USHORT EntryId)
{
	UINT InBootOrderLength = 1;
	UINT OutBootOrderLength = 1;
	USHORT* InBootOrder = new USHORT[InBootOrderLength];
	USHORT* OutBootOrder;
	InBootOrder[0] = 0;
	BOOL RetVal = FALSE;
	UINT RetLen = 0;
	//
	while ((RetLen = GetFirmwareEnvironmentVariable(TEXT("BootOrder"), EfiGuid, InBootOrder, (InBootOrderLength * sizeof(USHORT)))) == 0) {
		if (GetLastError() == ERROR_ENVVAR_NOT_FOUND) {
			InBootOrderLength = 0;
			break;
		}
		InBootOrderLength++;
		delete[] InBootOrder;
		InBootOrder = new USHORT[InBootOrderLength];
	}
	OutBootOrder = new USHORT[USHRT_MAX];
	memset(OutBootOrder, 0, USHRT_MAX * sizeof(USHORT));
	OutBootOrder[0] = EntryId;
	if (InBootOrderLength) {
		for (UINT i = 0; i <= InBootOrderLength-1; i++) {
			if (InBootOrder[i] == EntryId) {
				continue;
			}
			else {
				OutBootOrder[OutBootOrderLength] = InBootOrder[i];
				OutBootOrderLength++;
			}
		}
	}
	RetVal = SetFirmwareEnvironmentVariable(TEXT("BootOrder"), EfiGuid, OutBootOrder, (OutBootOrderLength * sizeof(USHORT)));
	delete[] InBootOrder;
	delete[] OutBootOrder;
	return RetVal;
}
//
BOOL
	EfiBootOrderDelete(USHORT EntryId)
{
	UINT InBootOrderLength = 1;
	UINT OutBootOrderLength = 0;
	USHORT* InBootOrder = new USHORT[InBootOrderLength];
	USHORT* OutBootOrder;
	BOOL RetVal = FALSE;
	UINT RetLen = 0;
	//
	while ((RetLen = GetFirmwareEnvironmentVariable(TEXT("BootOrder"), EfiGuid, InBootOrder, (InBootOrderLength * sizeof(USHORT)))) == 0) {
		if (GetLastError() == ERROR_ENVVAR_NOT_FOUND) {
			InBootOrderLength = 0;
			break;
		}
		InBootOrderLength++;
		delete[] InBootOrder;
		InBootOrder = new USHORT[InBootOrderLength];
	}
	OutBootOrder = new USHORT[USHRT_MAX];
	memset(OutBootOrder, 0, USHRT_MAX * sizeof(USHORT));
	if (InBootOrderLength) {
		for (UINT i = 0; i <= InBootOrderLength-1; i++) {
			if (InBootOrder[i] == EntryId) {
				continue;
			}
			else {
				OutBootOrder[OutBootOrderLength] = InBootOrder[i];
				OutBootOrderLength++;
			}
		}
		RetVal = SetFirmwareEnvironmentVariable(TEXT("BootOrder"), EfiGuid, OutBootOrder, (OutBootOrderLength * sizeof(USHORT)));
	}
	delete[] InBootOrder;
	delete[] OutBootOrder;
	return RetVal;
}
//
BOOL
	EfiBootmgrAddFirst(VOID)
{
	wchar_t FilePath[] = L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi";
	ULONG BufferLength = 16;
	UCHAR* Buffer = new UCHAR[BufferLength];
	memset(Buffer, 0, BufferLength);
	wchar_t EntryName[24] = {};
	USHORT EntryId;
	UINT InBootOrderLength = 1;
	USHORT* InBootOrder = new USHORT[InBootOrderLength];
	InBootOrder[0] = 0;
	BOOL RetVal = FALSE;
	UINT RetLen = 0;
	//
	while ((RetLen = GetFirmwareEnvironmentVariable(TEXT("BootOrder"), EfiGuid, InBootOrder, InBootOrderLength * sizeof(USHORT))) == 0) {
		if (GetLastError() == ERROR_ENVVAR_NOT_FOUND) {
			InBootOrderLength = 0;
			break;
		}
		InBootOrderLength++;
		delete[] InBootOrder;
		InBootOrder = new USHORT[InBootOrderLength];
	}
	if (InBootOrderLength) {
		for (UINT i = 0; i <= InBootOrderLength - 1; i++) {
			BufferLength = 16;
			delete[] Buffer;
			Buffer = new UCHAR[BufferLength];
			memset(Buffer, 0, BufferLength);
			wsprintf(EntryName, L"Boot%04X", InBootOrder[i]);
			EntryId = InBootOrder[i];
			while ((RetLen = GetFirmwareEnvironmentVariable(EntryName, EfiGuid, Buffer, BufferLength)) == 0) {
				if (GetLastError() == ERROR_ENVVAR_NOT_FOUND) {
					break;
				}
				BufferLength += 16;
				delete[] Buffer;
				Buffer = new UCHAR[BufferLength];
				memset(Buffer, 0, BufferLength);
			}
			if (RetLen >= sizeof(FilePath)) {
				for (UINT j = 0; j < (RetLen - sizeof(FilePath) - 1); j++) {
					if (memcmp((Buffer + j), FilePath, sizeof(FilePath) - 1) == 0) {
						RetVal = EfiBootOrderAddFirst(EntryId);
					}
				}
			}
		}
	}
	delete[] InBootOrder;
	delete[] Buffer;
	return RetVal;
}
//
UINT
	EfiDeleteWindSLICEntries(VOID)
{
	wchar_t FilePath[] = L"WindSLIC";
	ULONG BufferLength = 16;
	UCHAR* Buffer = new UCHAR[BufferLength];
	memset(Buffer, 0, BufferLength);
	wchar_t EntryName[24] = {};
	USHORT EntryId;
	UINT InBootOrderLength = 1;
	USHORT* InBootOrder = new USHORT[InBootOrderLength];
	InBootOrder[0] = 0;
	UINT RetVal = 0;
	UINT RetLen = 0;
	//
	while ((RetLen = GetFirmwareEnvironmentVariable(TEXT("BootOrder"), EfiGuid, InBootOrder, InBootOrderLength * sizeof(USHORT))) == 0) {
		if (GetLastError() == ERROR_ENVVAR_NOT_FOUND) {
			InBootOrderLength = 0;
			break;
		}
		InBootOrderLength++;
		delete[] InBootOrder;
		InBootOrder = new USHORT[InBootOrderLength];
	}
	if (InBootOrderLength) {
		for (UINT i = 0; i <= InBootOrderLength - 1; i++) {
			BufferLength = 16;
			delete[] Buffer;
			Buffer = new UCHAR[BufferLength];
			memset(Buffer, 0, BufferLength);
			wsprintf(EntryName, L"Boot%04X", InBootOrder[i]);
			EntryId = InBootOrder[i];
			while ((RetLen = GetFirmwareEnvironmentVariable(EntryName, EfiGuid, Buffer, BufferLength)) == 0) {
				if (GetLastError() == ERROR_ENVVAR_NOT_FOUND) {
					break;
				}
				BufferLength += 16;
				delete[] Buffer;
				Buffer = new UCHAR[BufferLength];
				memset(Buffer, 0, BufferLength);
			}
			if (RetLen >= sizeof(FilePath)) {
				for (UINT j = 0; j < (RetLen - sizeof(FilePath) - 1); j++) {
					if (memcmp((Buffer + j), FilePath, sizeof(FilePath) - 1) == 0) {
						EfiDeleteBootEntry(EntryName);
						EfiBootOrderDelete(EntryId);
						RetVal++;
					}
				}
			}
		}
	}
	delete[] InBootOrder;
	delete[] Buffer;
	return RetVal;
}
//
UINT
	EfiDeleteDescription(wchar_t* Description)
{
	ULONG BufferLength = 16;
	UCHAR* Buffer = new UCHAR[BufferLength];
	memset(Buffer, 0, BufferLength);
	wchar_t EntryName[24] = {};
	USHORT EntryId;
	UINT InBootOrderLength = 1;
	USHORT* InBootOrder = new USHORT[InBootOrderLength];
	InBootOrder[0] = 0;
	UINT RetVal = 0;
	UINT RetLen = 0;
	UINT DescriptionLenght = wcslen(Description);
	//
	while ((RetLen = GetFirmwareEnvironmentVariable(TEXT("BootOrder"), EfiGuid, InBootOrder, InBootOrderLength * sizeof(USHORT))) == 0) {
		if (GetLastError() == ERROR_ENVVAR_NOT_FOUND) {
			InBootOrderLength = 0;
			break;
		}
		InBootOrderLength++;
		delete[] InBootOrder;
		InBootOrder = new USHORT[InBootOrderLength];
	}
	if (InBootOrderLength) {
		for (UINT i = 0; i <= InBootOrderLength - 1; i++) {
			BufferLength = 16;
			delete[] Buffer;
			Buffer = new UCHAR[BufferLength];
			memset(Buffer, 0, BufferLength);
			wsprintf(EntryName, L"Boot%04X", InBootOrder[i]);;
			EntryId = InBootOrder[i];
			while ((RetLen = GetFirmwareEnvironmentVariable(EntryName, EfiGuid, Buffer, BufferLength)) == 0) {
				if (GetLastError() == ERROR_ENVVAR_NOT_FOUND) {
					break;
				}
				BufferLength += 16;
				delete[] Buffer;
				Buffer = new UCHAR[BufferLength];
				memset(Buffer, 0, BufferLength);
			}
			if (RetLen >= DescriptionLenght) {
				for (UINT j = 0; j < (RetLen - DescriptionLenght - 1); j++) {
					if (memcmp((Buffer + j), Description, DescriptionLenght - 1) == 0) {
						EfiDeleteBootEntry(EntryName);
						EfiBootOrderDelete(EntryId);
						RetVal++;
					}
				}
			}
		}
	}
	delete[] InBootOrder;
	delete[] Buffer;
	return RetVal;
}
//
BOOL
	EfiDeleteBootEntry (USHORT EntryId)
{
	wchar_t EntryName[24] = {};
	wsprintf(EntryName, L"Boot%04X", EntryId);
	return SetFirmwareEnvironmentVariable(EntryName, EfiGuid, NULL, NULL);
}
//
BOOL
	EfiDeleteBootEntry (wchar_t* EntryName)
{
	return SetFirmwareEnvironmentVariable(EntryName, EfiGuid, NULL, NULL);
}
//
USHORT
	EfiFreeBootEntry (VOID)
{
	UINT InBootOrderLength = 1;
	USHORT* InBootOrder = new USHORT[InBootOrderLength];
	InBootOrder[0] = 0;
	UINT* index = new UINT[USHRT_MAX];
	memset(index, 0, USHRT_MAX * sizeof(UINT));
	UINT RetLen = 0;
	//
	while ((RetLen = GetFirmwareEnvironmentVariable(TEXT("BootOrder"), EfiGuid, InBootOrder, (InBootOrderLength * sizeof(USHORT)))) == 0) {
		if (GetLastError() == ERROR_ENVVAR_NOT_FOUND) {
			delete[] InBootOrder;
			return 0;
		}
		InBootOrderLength++;
		delete[] InBootOrder;
		InBootOrder = new USHORT[InBootOrderLength];
	}
	for (UINT i = 0; i < InBootOrderLength; i++) {
		++index[InBootOrder[i]];
	}
	for (UINT i = 0; i <= USHRT_MAX - 1; i++) {
		if (!index[i]) {
			delete[] InBootOrder;
			delete[] index;
			return i;
		}
	}
	delete[] InBootOrder;
	delete[] index;
	return -1;
}
//
int
	EfiCreateBootEntry(UCHAR* pBootEntry, ULONG* pBootEntryLength, wchar_t* Description, wchar_t* FilePath)
{
	UCHAR* Buffer = NULL;
	ULONG BufferLength = NULL;
	HANDLE hDevice = NULL;
	DWORD BytesReturned = NULL;
	DISK_GEOMETRY* pDiskGeometry = NULL;
	size_t BootEntrySize = NULL;
	PEFI_BOOT_ENTRY pEfiBootEntry = NULL;
	PEFI_HARDDRIVE_DEVPATH pEfiDevPath = NULL;
	wchar_t* VolumeName = NULL;
	wchar_t* DriveName = NULL;
	PVOLUME_DISK_EXTENTS pDiskExtents = NULL;
	UINT32 BytesPerSector = NULL;
	PDRIVE_LAYOUT_INFORMATION_EX pDriveLayoutInfo = NULL;
	size_t EfiFilePathSize = NULL;
	PEFI_FILE_PATH pEfiFilePath = NULL;
	//
	if (Description == NULL || FilePath == NULL) {
		return ERROR_INVALID_PARAMETER;
	}
	try {
		if (!InitLib(TRUE)) {
			return ERROR_INVALID_HANDLE;
		}
		VolumeName = new wchar_t[MAX_PATH];
		DriveName = new wchar_t[MAX_PATH];
		// first part of boot entry.
		BootEntrySize = sizeof(EFI_BOOT_ENTRY) + (wcslen(Description) * 2);
		pEfiBootEntry = (PEFI_BOOT_ENTRY) malloc(BootEntrySize);
		if(!pEfiBootEntry) {
			throw bad_alloc();
		}
		memset(pEfiBootEntry, 0, BootEntrySize);
		pEfiBootEntry->Attributes = 1;
		wcsncpy_s((wchar_t*) pEfiBootEntry->Description, wcslen(Description) + 1, Description, wcslen(Description));
		// second part of boot entry.
		pEfiDevPath = (PEFI_HARDDRIVE_DEVPATH) malloc(sizeof(EFI_HARDDRIVE_DEVPATH));
		if(!pEfiDevPath) {
			throw bad_alloc();
		}
		memset(pEfiDevPath, 0, sizeof(EFI_HARDDRIVE_DEVPATH));
		pEfiDevPath->Type = 4;
		pEfiDevPath->SubType = 1;
		pEfiDevPath->Length = sizeof(EFI_HARDDRIVE_DEVPATH);
		// get system volume.
		BufferLength = 128;
		Buffer = new UCHAR[BufferLength];
		memset(Buffer, 0, BufferLength);
		pNtQuerySystemInformation((SYSTEM_INFORMATION_CLASS) 98, Buffer, BufferLength, &BufferLength);
		wsprintf(VolumeName, L"\\\\.\\%s\0", (wchar_t*)(Buffer + 24));
		hDevice = CreateFile(VolumeName, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
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
		// get bytes per sector.
		BufferLength = 16;
		Buffer = new UCHAR[BufferLength];
		memset(Buffer, 0, BufferLength);
		BytesReturned = 0;
		while (!DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, NULL, Buffer, BufferLength, &BytesReturned, NULL)) {
			BufferLength += 16;
			delete[] Buffer;
			Buffer = new UCHAR[BufferLength];
			memset(Buffer, 0, BufferLength);
		}
		pDiskGeometry = (DISK_GEOMETRY *) Buffer;
		BytesPerSector = pDiskGeometry->BytesPerSector;
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
		// add ESP info to boot entry.
		pDriveLayoutInfo = (PDRIVE_LAYOUT_INFORMATION_EX) Buffer;
		for (UINT i = 0; i < pDriveLayoutInfo->PartitionCount; i++) {
			if (pDriveLayoutInfo->PartitionStyle == PARTITION_STYLE_GPT) {
				if (memcmp(&pDriveLayoutInfo->PartitionEntry[i].Gpt.PartitionType, &ESP, sizeof(GUID)) == 0) {
					pEfiDevPath->PartitionNumber = pDriveLayoutInfo->PartitionEntry[i].PartitionNumber;
					pEfiDevPath->PartitionStart = pDriveLayoutInfo->PartitionEntry[i].StartingOffset.QuadPart / BytesPerSector;
					pEfiDevPath->PartitionSize = pDriveLayoutInfo->PartitionEntry[i].PartitionLength.QuadPart / BytesPerSector;
					memcpy(pEfiDevPath->PartitionSig, &pDriveLayoutInfo->PartitionEntry[i].Gpt.PartitionId, sizeof(GUID));
					pEfiDevPath->PartitionFormat = GPT;
					pEfiDevPath->SignatureType = GPT;
				}
			}
			else {
				free(pEfiBootEntry);
				free(pEfiDevPath);
				delete[] Buffer;
				delete[] DriveName;
				delete[] VolumeName;
				return ERROR_CALL_NOT_IMPLEMENTED;
			}
		}
		delete[] Buffer;
		// third part of boot entry.
		EfiFilePathSize = sizeof(EFI_FILE_PATH) + (wcslen(FilePath) * 2);
		pEfiFilePath = (PEFI_FILE_PATH) malloc(EfiFilePathSize);
		if(!pEfiFilePath) {
			throw bad_alloc();
		}
		memset(pEfiFilePath, 0, EfiFilePathSize);
		pEfiFilePath->Type = 4;
		pEfiFilePath->SubType = 4;
		pEfiFilePath->Length = EfiFilePathSize;
		wcsncpy_s((wchar_t *) pEfiFilePath->Path, wcslen(FilePath) + 1, FilePath, wcslen(FilePath));
		// finaly build the boot entry.
		BufferLength = BootEntrySize + sizeof(EFI_HARDDRIVE_DEVPATH) + EfiFilePathSize + 4;
		if (*pBootEntryLength < BufferLength || pBootEntry == NULL) {
			*pBootEntryLength = BufferLength;
			free(pEfiBootEntry);
			free(pEfiDevPath);
			free(pEfiFilePath);
			delete[] DriveName;
			delete[] VolumeName;
			return ERROR_INSUFFICIENT_BUFFER;
		}
		*pBootEntryLength = BufferLength;
		pEfiBootEntry->FilePathLength = sizeof(EFI_HARDDRIVE_DEVPATH) + EfiFilePathSize + 4;
		memcpy(pBootEntry, pEfiBootEntry, BootEntrySize);
		memcpy(pBootEntry + BootEntrySize, pEfiDevPath, sizeof(EFI_HARDDRIVE_DEVPATH));
		memcpy(pBootEntry + BootEntrySize + sizeof(EFI_HARDDRIVE_DEVPATH), pEfiFilePath, EfiFilePathSize);
		pBootEntry[BufferLength - 1] = 0;
		pBootEntry[BufferLength - 2] = 4;
		pBootEntry[BufferLength - 3] = 0xFF;
		pBootEntry[BufferLength - 4] = 0x7F;
		free(pEfiBootEntry);
		free(pEfiDevPath);
		free(pEfiFilePath);
		delete[] DriveName;
		delete[] VolumeName;
		return ERROR_SUCCESS;
	}
	catch (bad_alloc) {
		if(NULL != DriveName) {delete[] DriveName;}
		if(NULL != VolumeName) {delete[] VolumeName;}
		if(NULL != pEfiBootEntry) {free(pEfiBootEntry);}
		if(NULL != pEfiDevPath) {free(pEfiDevPath);}
		if(NULL != pEfiFilePath) {free(pEfiFilePath);}
		return ERROR_OUTOFMEMORY;
	}
}
//
BOOL
	AcquirePrivilage(VOID)
{
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb530716(v=vs.85).aspx
	TOKEN_PRIVILEGES NewState;
	LUID luid;
	HANDLE hToken = NULL;
	//
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY|TOKEN_QUERY_SOURCE, &hToken)) {
		CloseHandle(hToken);
		return FALSE;
	}
	if (!LookupPrivilegeValue(NULL, TEXT("SeSystemEnvironmentPrivilege"), &luid)) {
		CloseHandle(hToken);
		return FALSE;
	}
	NewState.PrivilegeCount = 1;
	NewState.Privileges[0].Luid = luid;
	NewState.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &NewState, sizeof(NewState), NULL, NULL)) {
		CloseHandle(hToken);
		return FALSE;
	}
	CloseHandle(hToken);
	return TRUE;
}
//
BOOL
isEfi(VOID)
{
	BOOL RetVal = FALSE;
	if (!InitLib(TRUE)) {
		return RetVal;
	}
	if (IsWindows8OrGreater()) {
		FIRMWARE_TYPE FirmwareType;
		pGetFirmwareType(&FirmwareType);
		RetVal = FirmwareType == FirmwareTypeUefi;
	}
	else if (IsWindows7OrGreater()) {
		DWORD buffer[5] = {};
		if (pNtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)90, buffer, sizeof(buffer), NULL) == 0 && buffer[4] == 2) {
			RetVal = TRUE;
		}
	}
	return RetVal;
}
//
BOOL
	GetFreeLetter (LPWSTR lpDeviceName)
{
	BOOL RetVal = FALSE;
	DWORD dwDrives = GetLogicalDrives();
	DWORD dwMask = 1;
	for (int i = 0; i < 26; i++) {
		if (i > 2) {
			if (dwDrives & dwMask) {
			}
			else {
				wsprintf((LPWSTR)lpDeviceName, L"%c:", 'A' + i);
				RetVal = TRUE;
				break;
			}
		}
		dwMask <<= 1;
	}
	return RetVal;
}
//
BOOL
	MountEsp(wchar_t* DosDevice)
{
	BOOL RetVal = FALSE;
	if (!InitLib(TRUE)) {
		return RetVal;
	}
	UCHAR* Buffer = new UCHAR[128];
	memset(Buffer, 0, 128);
	if(pNtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)98, Buffer, 128, NULL) == 0) {
		UINT strlen = wcslen((wchar_t *)(Buffer + 8)) + 1;
		wchar_t* pTargetPath = new wchar_t[strlen];
		wcscpy_s(pTargetPath, strlen, (wchar_t *)(Buffer + 8));
		RetVal = DefineDosDeviceW(DDD_RAW_TARGET_PATH|DDD_NO_BROADCAST_SYSTEM, DosDevice, pTargetPath);
		delete[] pTargetPath;
	}
	delete[] Buffer;
	return RetVal;
}
//
BOOL
	UnmountEsp(wchar_t* DosDevice)
{
	BOOL RetVal = FALSE;
	if (!InitLib(TRUE)) {
		return RetVal;
	}
	UCHAR* Buffer = new UCHAR[128];
	memset(Buffer, 0, 128);
	if(pNtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)98, Buffer, 128, NULL) == 0) {
		UINT strlen = wcslen((wchar_t *)(Buffer + 8)) + 1;
		wchar_t* pTargetPath = new wchar_t[strlen];
		wcscpy_s(pTargetPath, strlen, (wchar_t *)(Buffer + 8));
		RetVal = DefineDosDeviceW(DDD_RAW_TARGET_PATH|DDD_REMOVE_DEFINITION|DDD_EXACT_MATCH_ON_REMOVE, DosDevice, pTargetPath);
		delete[] pTargetPath;
	}
	delete[] Buffer;
	return RetVal;
}
//
