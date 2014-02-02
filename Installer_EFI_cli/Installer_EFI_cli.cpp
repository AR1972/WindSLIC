//
/* This program is free software. It comes without any warranty, to
* the extent permitted by applicable law. You can redistribute it
* and/or modify it under the terms of the Do What The Fuck You Want
* To Public License, Version 2, as published by Sam Hocevar. See
* http://sam.zoy.org/wtfpl/COPYING for more details. */
//
#include "stdafx.h"
using namespace std;
#define MAX_SLPSTRING 32

int _tmain(int argc, _TCHAR* argv[])
{
	ofstream outfile;
	int ret = 1;
	UCHAR Key[16] = {};
	wchar_t EntryName[24] = {};
	wchar_t Description[] = L"WindSLIC\0";
	wchar_t BootFile[] = L"\\BOOTX64.EFI\0";
	ULONG BootEntryLength = 0;
	HRSRC hResource;
	HGLOBAL hgResource;
	size_t WindSlicSize;
	size_t SlicSize;
	ULONG EntryId;
	UCHAR* WindSlicBuffer;
	UCHAR* SlicBuffer;
	UCHAR* BootEntry;
	wchar_t* UId = NULL;
	CHAR* SLPString;
	UINT32 SLPLength = 0;
	UINT32 ScanBegin = 0;
	UINT32 ScanEnd = 0;
	wchar_t* DosDevice = NULL;
	wchar_t* Directory = NULL;
	wchar_t* OldDirectory = NULL;
	wchar_t* FilePath = NULL;
	wchar_t* BootEntryPath = NULL;
	//
	if (!InitLib(TRUE)) {
		wprintf_s(L"lib initialization error\n");
		return 1;
	}
	//
	if (!isEfi()) {
		wprintf_s(L"system is not EFI\n");
		return 2;
	}
	//
	if (!Test()) {
		wprintf_s(L"EFI variable error\n");
		return 1;
	}
	//
	try {
		DosDevice = new wchar_t[8];
		Directory = new wchar_t[MAX_PATH];
		OldDirectory = new wchar_t[MAX_PATH];
		FilePath = new wchar_t[MAX_PATH];
		BootEntryPath = new wchar_t[MAX_PATH];
		UId = new wchar_t[sizeof(GUID) * 2 + 4]();
		GetFreeLetter (DosDevice);
		if(UniqueId(&UId) == ERROR_OUTOFMEMORY) {
			throw bad_alloc();
		}
		wsprintf(Directory, L"%s\\EFI\\%s\0", DosDevice, UId);
		wsprintf(OldDirectory, L"%s\\EFI\\WINDSLIC\0", DosDevice);
		wsprintf(FilePath, L"%s%s\0", Directory, BootFile);
		wsprintf(BootEntryPath, L"\\EFI\\%s%s\0", UId, BootFile);
		// get modify firmware variable privilage.
		AcquirePrivilage();
		// install WindSLIC to ESP.
		MountEsp(DosDevice);
		DeleteRecursive(OldDirectory);
		while (DeleteRecursive(Directory) != 0) {
			Sleep(100);
		}
		if (argv[1] != NULL && wcscmp(argv[1], L"/u\0") == 0) {
			wprintf_s(L"uninstalling WindSLIC\n");
			EfiDeleteWindSLICEntries();
			EfiBootmgrAddFirst();
			goto exit;
		}
		if (CreateDirectory(Directory, NULL)) {
			wprintf_s(L"installing WindSLIC to: %s\n", FilePath);
			// get WindSLIC resource.
			hResource = FindResource(NULL, MAKEINTRESOURCE(101), L"RAW");
			WindSlicSize = SizeofResource(NULL, hResource);
			hgResource = LoadResource(NULL, hResource);
			WindSlicBuffer = new UCHAR[WindSlicSize];
			memcpy(WindSlicBuffer, LockResource(hgResource), WindSlicSize);
			// get SLIC resource.
			hResource = FindResource(NULL, MAKEINTRESOURCE(102), L"RAW");
			SlicSize = SizeofResource(NULL, hResource);
			SlicBuffer = new UCHAR[SlicSize];
			hgResource = LoadResource(NULL, hResource);
			memcpy(SlicBuffer,LockResource(hgResource), SlicSize);
			SLPString = "AcerSystem\0";
			ScanBegin = 0x0F0000;
			ScanEnd = 0x0FFFFF;
			SLPLength = strlen(SLPString);
			// generate encryption key.
			srand((UINT)time(0));
			for (UINT i = 0; i < sizeof(Key); i++) {
				Key[i] = (UCHAR)(rand() % 0x5D) + 0x21;
			}
			// encrypt SLIC.
			UINT pos = sizeof(Key)-1;
			for(UINT i = 0; i < SlicSize; i++) {
				SlicBuffer[i] ^= Key[pos];
				pos--;
				if (pos == -1) {
					pos = sizeof(Key)-1;
				}
			}
			// encrypt key.
			for (UINT i = 0; i < sizeof(Key); i++) {
				Key[i] ^= 0xFF;
			}
			// copy encrypted data.
			for (UINT i = 0; i < WindSlicSize - 6; i++) {
				BOOL slic = FALSE;
				BOOL key = FALSE;
				BOOL slp = FALSE;
				BOOL slplen = FALSE;
				BOOL begin = FALSE;
				BOOL end = FALSE;
				//
				if (WindSlicBuffer[i] == 0x53 && WindSlicBuffer[i+1] == 0x4C
					&& WindSlicBuffer[i+2] == 0x50 && WindSlicBuffer[i+3] == 0x53
					&& WindSlicBuffer[i+4] == 0x54 && WindSlicBuffer[i+5] == 0x52) {
						memset(&WindSlicBuffer[i], 0, MAX_SLPSTRING);
						memcpy(&WindSlicBuffer[i], SLPString, SLPLength);
						slp = TRUE;
				}
				if (WindSlicBuffer[i] == 0x4B && WindSlicBuffer[i+1] == 0x45
					&& WindSlicBuffer[i+2] == 0x59 && WindSlicBuffer[i+3] == 0x4B
					&& WindSlicBuffer[i+4] == 0x45 && WindSlicBuffer[i+5] == 0x59) {
						memcpy(&WindSlicBuffer[i], Key, sizeof(Key));
						key = TRUE;
				}
				if (WindSlicBuffer[i] == 0x53 && WindSlicBuffer[i+1] == 0x4C
					&& WindSlicBuffer[i+2] == 0x49 && WindSlicBuffer[i+3] == 0x43
					&& WindSlicBuffer[i+4] == 0x76 && WindSlicBuffer[i+5] == 0x01) {
						memcpy(&WindSlicBuffer[i], SlicBuffer, SlicSize);
						slic = TRUE;
				}
				if (WindSlicBuffer[i] == 0x41 && WindSlicBuffer[i+1] == 0x41
					&& WindSlicBuffer[i+2] == 0x41 && WindSlicBuffer[i+3] == 0x41) {
						memcpy(&WindSlicBuffer[i], &SLPLength, sizeof(UINT32));
						slplen = TRUE;
				}
				if (WindSlicBuffer[i] == 0x42 && WindSlicBuffer[i+1] == 0x42
					&& WindSlicBuffer[i+2] == 0x42 && WindSlicBuffer[i+3] == 0x42) {
						memcpy(&WindSlicBuffer[i], &ScanBegin, sizeof(UINT32));
						begin = TRUE;
				}
				if (WindSlicBuffer[i] == 0x43 && WindSlicBuffer[i+1] == 0x43
					&& WindSlicBuffer[i+2] == 0x43 && WindSlicBuffer[i+3] == 0x43) {
						memcpy(&WindSlicBuffer[i], &ScanEnd, sizeof(UINT32)); 
						end = TRUE;
				}
				if (key && slic && slp && end && begin && slplen) {
					break;
				}
			}
			outfile.open(FilePath, ios_base::binary);
			if (outfile.is_open()) {
				outfile.write((char*) WindSlicBuffer, WindSlicSize);
				outfile.close();
			}
			delete[] WindSlicBuffer;
			delete[] SlicBuffer;
		}
		else {
			wprintf_s(L"create directory failed: %X\n", GetLastError());
			EfiDeleteWindSLICEntries();
			EfiBootmgrAddFirst();
			goto exit;
		}
		// remove any WindSLIC boot entries.
		EfiDeleteWindSLICEntries();
		EfiBootmgrAddFirst();
		// add WindSLIC boot entry.
		EntryId = EfiFreeBootEntry();
		if (EntryId == -1) {
			wprintf_s(L"find free boot entry failed");
		}
		else {
			wsprintf(EntryName, L"Boot%04d", EntryId);
			if(EfiCreateBootEntry(NULL, &BootEntryLength, Description, BootEntryPath) == ERROR_OUTOFMEMORY) {
				throw bad_alloc();
			}
			BootEntry = new UCHAR[BootEntryLength];
			switch(EfiCreateBootEntry(BootEntry, &BootEntryLength, Description, BootEntryPath)) {
			case ERROR_SUCCESS:
				wprintf_s(L"adding WindSLIC boot entry: %s\n", EntryName);
				SetFirmwareEnvironmentVariable(EntryName, EfiGuid, BootEntry, BootEntryLength);
				EfiBootOrderAddFirst(EntryId);
				break;
			case ERROR_OUTOFMEMORY:
				throw bad_alloc();
				break;
			}
			ret = ERROR_SUCCESS;
			delete[] BootEntry;
		}
exit:
		UnmountEsp(DosDevice);
		delete[] UId;
		delete[] BootEntryPath;
		delete[] FilePath;
		delete[] Directory;
		delete[] OldDirectory;
		delete[] DosDevice;
		InitLib(FALSE);
		return ret;
	}
	catch (bad_alloc) {
		if(NULL != UId) {delete[] UId;}
		if(NULL != Directory) {delete[] Directory;}
		if(NULL != OldDirectory) {delete[] OldDirectory;}
		if(NULL != DosDevice) {delete[] DosDevice;}
		if(NULL != FilePath) {delete[] FilePath;}
		if(NULL != BootEntryPath) {delete[] BootEntryPath;}
		InitLib(FALSE);
		return ERROR_OUTOFMEMORY;
	}
}

