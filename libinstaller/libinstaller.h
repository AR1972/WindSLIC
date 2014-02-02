#ifndef LIBINSTALLER
#define LIBINSTALLER
//
/* This program is free software. It comes without any warranty, to
* the extent permitted by applicable law. You can redistribute it
* and/or modify it under the terms of the Do What The Fuck You Want
* To Public License, Version 2, as published by Sam Hocevar. See
* http://sam.zoy.org/wtfpl/COPYING for more details. */
//
#pragma once
#include "stdafx.h"
#pragma pack(2)
//
typedef NTSTATUS (WINAPI *QuerySystemInformation)(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength);
//
typedef struct _EFI_BOOT_ENTRY
{
	ULONG Attributes;
	USHORT FilePathLength;
	CHAR Description[1];
} EFI_BOOT_ENTRY, *PEFI_BOOT_ENTRY;
//
typedef struct _EFI_HARDDRIVE_DEVPATH
{
	UCHAR Type;				// 4 = Media Device Path
	UCHAR SubType;			// 1 = Hard Drive
	USHORT Length;			// 42
	UINT32 PartitionNumber;
	UINT64 PartitionStart;
	UINT64 PartitionSize;
	CHAR PartitionSig[16];
	UCHAR PartitionFormat;	// 2
	UCHAR SignatureType;	// 2
} EFI_HARDDRIVE_DEVPATH, *PEFI_HARDDRIVE_DEVPATH;
//
typedef struct _EFI_FILE_PATH
{
	UCHAR Type;				// 4 = Media Device Path
	UCHAR SubType;			// 4 = File Path
	USHORT Length;
	CHAR Path[1];
} EFI_FILE_PATH, *PEFI_FILE_PATH;
//
typedef enum ePartitionType {
	NONE,
	MBR,
	GPT
};
static wchar_t EfiGuid[] = L"{8BE4DF61-93CA-11d2-AA0D-00E098032B8C}";
static UCHAR ESP[] = {0x28, 0x73, 0x2A, 0xC1, 0x1F, 0xF8, 0xD2, 0x11, 0xBA, 0x4B, 0x00, 0xA0, 0xC9, 0x3E, 0xC9, 0x3B};
static QuerySystemInformation pNtQuerySystemInformation = NULL;
static HMODULE hNtdll = NULL;
//
int 
	EfiCreateBootEntry(UCHAR* pBootEntry, ULONG* pBootEntryLength, wchar_t* Description, wchar_t* FilePath);
//
BOOL
	AcquirePrivilage(VOID);
//
ULONG
	EfiFreeBootEntry (VOID);
//
BOOL
	EfiDeleteBootEntry (ULONG EntryId);
//
BOOL
	EfiDeleteBootEntry (wchar_t* EntryName);
//
UINT
	EfiDeleteWindSLICEntries(VOID);
//
UINT
	EfiDeleteDescription(wchar_t* Description, int DescriptionLenght);
//
BOOL
	EfiBootOrderDelete(ULONG EntryId);
//
BOOL
	EfiBootOrderAddFirst(ULONG EntryId);
//
BOOL
	EfiBootmgrAddFirst(VOID);
//
BOOL 
	isEfi(VOID);
//
BOOL
	GetFreeLetter (LPWSTR lpDeviceName);
//
BOOL
	MountEsp(wchar_t* DosDevice);
//
BOOL
	UnmountEsp(wchar_t* DosDevice);
//
UINT
	DeleteRecursive(wchar_t* Directory);
//
int
	UniqueId(wchar_t** Id);
//
BOOL
	Test(VOID);
//
BOOL
	InitLib(BOOL Load);
#endif