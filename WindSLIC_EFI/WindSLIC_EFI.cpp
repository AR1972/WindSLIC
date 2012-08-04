//
/* This program is free software. It comes without any warranty, to
* the extent permitted by applicable law. You can redistribute it
* and/or modify it under the terms of the Do What The Fuck You Want
* To Public License, Version 2, as published by Sam Hocevar. See
* http://sam.zoy.org/wtfpl/COPYING for more details. */
//
// VERBOSE 0: no text printed to screen
// VERBOSE 1: info printed to screen
// VERBOSE 2: info + hex dumps printed to screen
//
#define VERBOSE 0
//
// SLP10 1: inject SLP 1.0 string
//
#define SLP10_INJECT 1
//
// MSDM 1: inject MSDM table
//
#define MSDM_INJECT 0
//
// BETA 1: enables incomplete patch in place code
//
#define BETA 0
//
// NOMEM 1: disables low memory unlocking
//
#define NOMEM 0
//
// NOENCRYPT 1: disables SLIC encryption
//
#define NOENCRYPT 0 /* Caution!! breaks installers */
//
#if NOMEM == 1 || NOENCRYPT == 1
#define SLP10_INJECT 0
#endif
//
#define  ASSERT(a)
//
#include <efi.h>
#include "base.h"
#include "acpi.h"
#include "libmtrr.h"
#include "libefi.h"
#include "WindSLIC_EFI.h"
#include "global.h"
#if NOENCRYPT == 0
#include "..\SLIC\dummy.h"
#else
#include "..\SLIC\acer.h"
#endif
#include "..\MSDM\msdm.h"
//
EFI_GUID gEfiAcpiTableGuid = ACPI_TABLE_GUID;
EFI_GUID gEfiAcpi20TableGuid = ACPI_20_TABLE_GUID;
EFI_GUID gLoadedImageProtocol = LOADED_IMAGE_PROTOCOL;
EFI_GUID gDevicePathProtocol = DEVICE_PATH_PROTOCOL;
EFI_GUID gFileSystemProtocol = SIMPLE_FILE_SYSTEM_PROTOCOL;
EFI_GUID gFileInfo = EFI_FILE_INFO_ID;
EFI_GUID gCpuProtocol = EFI_CPU_ARCH_PROTOCOL_GUID;
EFI_GUID gLegacyRegion = EFI_LEGACY_REGION_PROTOCOL_GUID;
//
#define LOWORD(l)           ((UINT16)(((UINT32)(l)) & 0xffff))
#define HIWORD(l)           ((UINT16)((((UINT32)(l)) >> 16) & 0xffff))
//
EFI_STATUS 
	main(
	EFI_HANDLE ImageHandle,
	EFI_SYSTEM_TABLE *SystemTable
	)
{
	EFI_STATUS Status = 0;
	INT32 i = 0;
	EFI_INPUT_KEY Key;
	BOOLEAN Inject = TRUE;
	/* loader */
	EFI_FILE_IO_INTERFACE *Vol = 0;
	EFI_FILE_HANDLE RootFs = 0;
	EFI_FILE_HANDLE FileHandle = 0;
	EFI_HANDLE* Search = 0;
	EFI_HANDLE DeviceHandle = 0;
	UINTN Size = 0;
	VOID* BootmgrBuffer = 0;
	EFI_FILE_INFO* FileInfoBuffer = 0;
	EFI_HANDLE BootmgrHandle = NULL;
	EFI_LOADED_IMAGE *BootmgrLoadedImage = 0;
	EFI_LOADED_IMAGE *LoadedImage = 0;
	CHAR16 *BOOTMGFW = L"\\EFI\\Microsoft\\BOOT\\BOOTMGFW.EFI";
	CHAR16 *BOOTMGFW_BAK = L"\\EFI\\Microsoft\\BOOT\\BOOTMGFW.BAK";
	Key.ScanCode = 0;
	Key.UnicodeChar = 0;

	//=========================================================================//
	// code starts                                                             //
	//=========================================================================//

	ST = SystemTable;
	BS = SystemTable->BootServices;
	RS = SystemTable->RuntimeServices;

	//=========================================================================//
	// set console mode                                                        //
	//=========================================================================//

	for (i = 0; i < ST->ConOut->Mode->MaxMode; i++) {
		if ((ST->ConOut->QueryMode(ST->ConOut, (UINTN)i, 0, 0) == EFI_SUCCESS) &&
			(i != ST->ConOut->Mode->Mode)) {
				ST->ConOut->SetMode(ST->ConOut, (UINTN)i);
		}
	}

	//=========================================================================//
	// disable blinking cursor                                                 //
	//=========================================================================//

	ST->ConOut->EnableCursor(ST->ConOut, FALSE);

	//=========================================================================//
	// detect F1 key                                                           //
	//=========================================================================//

	ST->ConIn->ReadKeyStroke (ST->ConIn, &Key);
	if (Key.UnicodeChar == 0 && Key.ScanCode == SCAN_F1) {
		Print(L"Press ESC key to boot Windows without WindSLIC\r\n");
		for (;;) {
			WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
			ST->ConIn->ReadKeyStroke(ST->ConIn, &Key);
			if (Key.UnicodeChar == CHAR_NULL && Key.ScanCode == SCAN_ESC) {
				Inject = FALSE;
				break;
			}
			if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
				Inject = TRUE;
				break;
			}
		}
	}

	//=========================================================================//
	// run injector code                                                       //
	//=========================================================================//

	if(Inject) {
		Status = Injector();
		if(EFI_ERROR(Status)) {
			/* stop & show user the error */
			ContinueKey(0);
		}
	}

	//=========================================================================//
	// get device handle for the loaded (this) image                           //
	//=========================================================================//

	Status = BS->HandleProtocol(ImageHandle, &gLoadedImageProtocol, (VOID **) &LoadedImage);
	if (EFI_ERROR (Status)) {
		Print(L"ERROR: Loaded Image Protocol: %r\r\n", Status);
		ContinueKey(0);
		return Status;
	}
	DeviceHandle = LoadedImage->DeviceHandle;

	//=========================================================================//
	// get file io interface for device image was loaded from                  //
	//=========================================================================//

	Status = BS->HandleProtocol(DeviceHandle, &gFileSystemProtocol, (VOID **) &Vol);
	if (EFI_ERROR (Status)) {
		Print(L"ERROR: File System Protocol: %r\r\n", Status);
		ContinueKey(0);
		return Status;
	}

	//=========================================================================//
	// open file system root for the device image was loaded from              //
	//=========================================================================//

	Status = Vol->OpenVolume(Vol, &RootFs);
	if (EFI_ERROR (Status)) {
		Print(L"ERROR: Open Volume: %s %r\r\n", Vol, Status);
		ContinueKey(0);
		return Status;
	}

	//=========================================================================//
	// try to open bootmgfw on file system that image was loaded from          //
	//=========================================================================//

#if VERBOSE > 0
	Print(L"Opening file: %s\r\n", BOOTMGFW);
#endif

	//=========================================================================//
	// look for BOOTMGFW.BAK first to support BOOTMGFW.EFI replacement         //
	// install method.                                                         //
	//=========================================================================//

	Status = RootFs->Open(RootFs, &FileHandle, BOOTMGFW_BAK, EFI_FILE_MODE_READ, 0);
	if (Status == EFI_SUCCESS) {
		BOOTMGFW = BOOTMGFW_BAK;
	}
	else if (Status == EFI_NOT_FOUND) {
		/* if BOOTMGFW.BAK not found search for BOOTMGFW.EFI */
		Status = RootFs->Open(RootFs, &FileHandle, BOOTMGFW, EFI_FILE_MODE_READ, 0);
		if (EFI_ERROR (Status)) {
			RootFs->Close(RootFs);
			switch(Status) {
			case EFI_NOT_FOUND:

				//=========================================================================//
				// failed to find bootmgfw on same device, look for it on other devices.   //
				// get array of device handle's that bootmgfw might be installed on        //
				// first get size of array                                                 //
				//=========================================================================//

				Size = 0;
				Status = BS->LocateHandle(ByProtocol, &gFileSystemProtocol, NULL, &Size, 0);
				if(Status == EFI_BUFFER_TOO_SMALL) {
					/* allocate memory for array */
					Search = (EFI_HANDLE *) AllocatePool(Size);
				}
				if(Search) {
					/* get the array */
					Status = BS->LocateHandle(ByProtocol, &gFileSystemProtocol, NULL, &Size, Search);
					/* loop through handle's open each file system & try to open bootmgfw */
					if(Status == EFI_SUCCESS) {
						for(i = 0; i < Size / sizeof(EFI_HANDLE); i++) {
							/* we already know bootmgfw is not in the same device as the loaded image, skip */
							if(Search[i] == DeviceHandle) {
								continue;
							}
							/* get file io interface */
							Status = BS->HandleProtocol(Search[i], &gFileSystemProtocol, (VOID **) &Vol);
							if(EFI_ERROR (Status)) {
								continue;
							}
							/* open file system root on the device */
							Status = Vol->OpenVolume(Vol, &RootFs);
							if(EFI_ERROR (Status)) {
								continue;
							}
							/* try to open bootmgfw on the file system */
							Status = RootFs->Open(RootFs, &FileHandle, BOOTMGFW, EFI_FILE_MODE_READ, 0);
							if(Status == EFI_SUCCESS) {
								/* found it, set DeviceHandle & break the loop */
								DeviceHandle = Search[i];
								break;
							}
							/* clean up for next pass */
							RootFs->Close(RootFs);
						}
						/* free array of device handles, if EFI_SUCCESS break */
						/* the switch/case else fall through to the error */
						BS->FreePool(Search);
						if(Status == EFI_SUCCESS) {
							break;
						}
					}
				}
			default:
				Print(L"ERROR: Open File: %s %r\r\n", BOOTMGFW, Status);
				ContinueKey(0);
				return Status;
			}
		}
	}
	else {
		Print(L"ERROR: Open File: %s %r\r\n", BOOTMGFW_BAK, Status);
		ContinueKey(0);
		return Status;
	}

	//============================================================================//
	// RootFs is open, FileHandle is open, DeviceHandle is set                    //
	// get size of bootmgfw.efi by retriving an EFI_FILE_INFO                     //
	// first get the size of the STRUCT and allocate memory                       //
	//============================================================================//

	Size = 0;
	Status = FileHandle->GetInfo(FileHandle, &gFileInfo, &Size, NULL);
	if(Status == EFI_BUFFER_TOO_SMALL) {
		/* allocate memory for EFI_FILE_INFO */
		FileInfoBuffer = (EFI_FILE_INFO *) AllocatePool(Size);
	} 
	else {
		Print(L"ERROR: File Size: %r\r\n", Status);
		ContinueKey(0);
		FileHandle->Close(FileHandle);
		RootFs->Close(RootFs);
		return Status;
	}

	//=========================================================================//
	// get EFI_FILE_INFO for bootmgfw.efi                                      //
	//=========================================================================//

	Status = FileHandle->GetInfo(FileHandle, &gFileInfo, &Size, FileInfoBuffer);
	if(EFI_ERROR(Status)) {
		Print(L"ERROR: File Size: %r\r\n", Status);
		ContinueKey(0);
		FileHandle->Close(FileHandle);
		RootFs->Close(RootFs);
		return Status;
	}

	//=========================================================================//
	// get size of bootmgfw.efi                                                //
	//=========================================================================//

	Size = FileInfoBuffer->FileSize;

	//=========================================================================//
	// free EFI_FILE_INFO buffer                                               //
	//=========================================================================//

	BS->FreePool(FileInfoBuffer);

	//=========================================================================//
	// allocate memory for bootmgfw.efi                                        //
	//=========================================================================//

	BootmgrBuffer = AllocatePool(Size);
	if (!BootmgrBuffer) {
		Print(L"ERROR: Allocate Memory: %r\r\n", Status);
		ContinueKey(0);
		BS->FreePool(BootmgrBuffer);
		FileHandle->Close(FileHandle);
		RootFs->Close(RootFs);
		return Status;
	}

	//=========================================================================//
	// read bootmgfw.efi into buffer                                           //
	//=========================================================================//

	Status = FileHandle->Read(FileHandle, &Size, BootmgrBuffer);
	if (EFI_ERROR (Status)) {
		Print(L"ERROR: Read File: %r\r\n", Status);
		ContinueKey(0);
		BS->FreePool(BootmgrBuffer);
		FileHandle->Close(FileHandle);
		RootFs->Close(RootFs);
		return Status;
	}

	//=========================================================================//
	// close handle for bootmgfw.efi                                           //
	//=========================================================================//

	Status = FileHandle->Close(FileHandle);
	if (EFI_ERROR (Status)) {
		Print(L"WARNING: Close File: %r\r\n", Status);
		ContinueKey(0);
	}

	//=========================================================================//
	// close handle for file system root                                       //
	//=========================================================================//

	Status = RootFs->Close(RootFs);
	if (EFI_ERROR (Status)) {
		Print(L"WARNING: Close Root: %r\r\n", Status);
		ContinueKey(0);
	}

	//=========================================================================//
	// load bootmgfw.efi from buffer to execution space                        //
	//=========================================================================//

	Status = BS->LoadImage(FALSE, ImageHandle, 0, BootmgrBuffer, Size, &BootmgrHandle);
	if (EFI_ERROR (Status)) {
		Print(L"ERROR: Load Image: %r\r\n", Status);
		ContinueKey(0);
		BS->FreePool(BootmgrBuffer);
		return Status;
	}
	BS->FreePool(BootmgrBuffer);

	//=========================================================================//
	// set bootmgfw.efi start variables                                        //
	//=========================================================================//

	Status = BS->HandleProtocol(BootmgrHandle, &gLoadedImageProtocol, (VOID **) &BootmgrLoadedImage);
	if (EFI_ERROR (Status)) {
		Print(L"ERROR: Loaded Image Protocol: %r\r\n", Status);
		ContinueKey(0);
		return Status;
	}
	BootmgrLoadedImage->DeviceHandle = DeviceHandle;
	BootmgrLoadedImage->ParentHandle = NULL;
	BootmgrLoadedImage->FilePath = FileDevicePath(DeviceHandle, BOOTMGFW);

	//=========================================================================//
	// start bootmgfw.efi execution                                            //
	//=========================================================================//

#if VERBOSE > 0
	Print(L"Press ENTER to start bootmgr\r\n");
	ContinueKey(30);
#endif

	Status = BS->StartImage(BootmgrHandle, 0, 0);

	//============================================================================//
	// should never get here show error                                           //
	//============================================================================//

	Print(L"ERROR: StartImage status %r\r\n", Status);
	if (BootmgrHandle != NULL) {
		Status = BS->UnloadImage(BootmgrHandle);
	}
	ContinueKey(30);
	return Status;
}
//
BOOLEAN
	ContinueKey (
	UINT64 Timeout OPTIONAL
	)
	/*
	Description:
	wait for user to press ENTER key or timeout
	*/
{
	EFI_INPUT_KEY  Key;
	EFI_STATUS     Status;
	/* Check for user intervention */
	for (;;) {
		Status = WaitForSingleEvent(ST->ConIn->WaitForKey, (Timeout * 10000000));
		if (Status == EFI_TIMEOUT) {
			return FALSE;
		}
		Status = ST->ConIn->ReadKeyStroke(ST->ConIn, &Key);
		if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
			return TRUE;
		}
	}
}
//
UINT8
	ComputeChecksum (
	UINT8	*Buffer,
	UINT32	Length
	)
	/*
	Description:
	Compute byte checksum on buffer of given length.

	Arguments:
	Buffer		- Pointer to buffer to compute checksum
	Length		- Number of bytes to checksum

	Returns:
	Checksum	- Checksum of buffer
	*/
{
	UINT8	Checksum = 0 ;
	while(Length--)
	{
		Checksum += *Buffer;
		Buffer++;
	}
	return -Checksum ;
}
//
EFI_STATUS
	GetSystemConfigurationTable (
	IN EFI_GUID *TableGuid,
	IN OUT VOID **Table
	)
	/*
	Description:
	Function returns a system configuration table that is stored in the 
	EFI System Table based on the provided GUID.

	Arguments:
	TableGuid        - A pointer to the table's GUID type.

	Table            - On exit, a pointer to a system configuration table.

	Returns:

	EFI_SUCCESS      - A configuration table matching TableGuid was found

	EFI_NOT_FOUND    - A configuration table matching TableGuid was not found
	*/
{
	UINTN Index;
	ASSERT (Table != NULL);
	//
	for (Index = 0; Index < ST->NumberOfTableEntries; Index++) {
		if (CompareGuid (TableGuid, &(ST->ConfigurationTable[Index].VendorGuid)) == 0) {
			*Table = ST->ConfigurationTable[Index].VendorTable;
			return EFI_SUCCESS;
		}
	}
	return EFI_NOT_FOUND;
}
//
VOID *
	FindAcpiRsdPtr (
	VOID
	)
	/*
	Description:
	finds the RSDP in low memory
	*/
{
	UINTN Address;
	UINTN Index;
	//
	// First Seach 0x0e0000 - 0x0fffff for RSD Ptr
	//
	for (Address = 0xe0000; Address < 0xfffff; Address += 0x10) {
		if (*(UINT64 *)(Address) == ACPI_RSDP_SIG) {
			return (VOID *)Address;
		}
	}
	//
	// Search EBDA
	//
	Address = (*(UINT16 *)(UINTN)(EBDA_BASE_ADDRESS)) << 4;
	for (Index = 0; Index < 0x400 ; Index += 16) {
		if (*(UINT64 *)(Address + Index) == ACPI_RSDP_SIG) {
			return (VOID *)Address;
		}
	}
	return NULL;
}
//
BOOLEAN
	ScanTableInRsdt (
	RSDTtbl_t *Rsdt,
	UINT32 Signature,
	VOID **FoundTable
	)
	/*
	Description:
	scans RSDT for a table Sig 
	*/
{
	UINT32 Count = 0;
	UINT32 i = 0;
	DESC_HEADER_t *Table = 0;
	*FoundTable = 0;
	//
	Count = (Rsdt->Header.Length - sizeof (DESC_HEADER_t)) >> 2;
	for (i = 0; i < Count; i++) {
		Table = (DESC_HEADER_t*) Rsdt->Entry[i];
		if (Table->Signature == Signature) {
			*FoundTable = Table;
			return TRUE;
		}
	}
	return FALSE;
}
//
BOOLEAN
	ScanTableInXsdt (
	XSDTtbl_t *Xsdt,
	UINT32 Signature,
	VOID **FoundTable
	)
	/*
	Description:
	scans XSDT for a table Sig
	*/
{
	UINT32 Count = 0;
	UINT32 i = 0;
	DESC_HEADER_t* Table = 0;
	*FoundTable = 0;

	Count = (Xsdt->Header.Length - sizeof (DESC_HEADER_t)) >> 3;
	for (i = 0; i < Count; i++) {
		Table = (DESC_HEADER_t*) Xsdt->Entry[i];
		if (Table->Signature == Signature) {
			*FoundTable = Table;
			return TRUE;
		}
	}
	return FALSE;
}
//
BOOLEAN
	UpdateRsdt (
	UINT32 Old_Address,
	UINT32 New_Address,
	RSDTtbl_t *Rsdt
	)
	/*
	Description:
	updates a table pointer in RSDT 
	useful when tables need to be moved
	*/
{
	UINT32 Count = 0;
	UINT32 i = 0;
	BOOLEAN Updated = FALSE;
	//
	Count = (Rsdt->Header.Length - sizeof (DESC_HEADER_t)) >> 2;
	for (i = 0; i < Count; i++) {
		if (Rsdt->Entry[i] == Old_Address) {
			Rsdt->Entry[i] = New_Address;
			Updated = TRUE;
		}
	}
	if (Updated) {
		Rsdt->Header.Checksum = 0;
		Rsdt->Header.Checksum = ComputeChecksum((UINT8 *) Rsdt, Rsdt->Header.Length);
	}
	return Updated;
}
//
BOOLEAN
	UpdateXsdt (
	UINT64 Old_Address,
	UINT64 New_Address,
	XSDTtbl_t *Xsdt
	)
	/*
	Description:
	updates table pointer in XSDT 
	useful when tables need to be moved
	*/
{
	UINT32 Count = 0;
	UINT32 i = 0;
	BOOLEAN Updated = FALSE;
	//
	Count = (Xsdt->Header.Length - sizeof (DESC_HEADER_t)) >> 3;
	for (i = 0; i < Count; i++) {
		if (Xsdt->Entry[i] == Old_Address) {
			Xsdt->Entry[i] = New_Address;
			Updated = TRUE;
		}
	}
	if(Updated) {
		Xsdt->Header.Checksum = 0;
		Xsdt->Header.Checksum = ComputeChecksum((UINT8 *) Xsdt, Xsdt->Header.Length);
	}
	return Updated;
}
//
DESC_HEADER_t *
	LowTableRsdt (
	RSDTtbl_t *Rsdt
	)
	/*
	Description:
	finds the table lowest in physical memory reletive to RSDT
	*/
{
	UINT32 Count = 0;
	UINT32 i = 0;
	DESC_HEADER_t *Table = 0;
	DESC_HEADER_t *Low_Table = 0;
	//
	Count = (Rsdt->Header.Length - sizeof (DESC_HEADER_t)) >> 2;
	for (i = 0; i < Count; i++) {
		Table = (DESC_HEADER_t *) Rsdt->Entry[i];
		if ((Low_Table == NULL) || ((Table < Low_Table) && (Table != 0))) {
			Low_Table = Table;
		}
	}
	return Low_Table;
}
//
DESC_HEADER_t *
	LowTableXsdt (
	XSDTtbl_t *Xsdt
	)
	/*
	Description:
	finds the table lowest in physical memory relative to XSDT
	*/
{
	UINT32 Count = 0;
	UINT32 i = 0;
	DESC_HEADER_t *Table = 0;
	DESC_HEADER_t *Low_Table = 0;
	// 
	Count = (Xsdt->Header.Length - sizeof (DESC_HEADER_t)) >> 3;
	for (i = 0; i < Count; i++) {
		Table = (DESC_HEADER_t *) Xsdt->Entry[i];
		if ((Low_Table == NULL) || ((Table < Low_Table) && (Table != 0))) {
			Low_Table = Table;
		}
	}
	return (DESC_HEADER_t*)((UINT64)Low_Table >> 32);
}
//
EFI_STATUS
	LegacyUnlock (
	VOID
	)
{
	EFI_STATUS Status = EFI_PROTOCOL_ERROR;
	EFI_LEGACY_REGION_PROTOCOL* LegacyRegionProtocol;
	UINT32 Granularity = 0;
	if(BS->LocateProtocol(&gLegacyRegion, NULL, (VOID**) &LegacyRegionProtocol) == EFI_SUCCESS) {
		if (LegacyRegionProtocol != NULL) {
			Status = LegacyRegionProtocol->UnLock(LegacyRegionProtocol, 0xF0000, 0xFFFF, &Granularity);
		}
	}
	return Status;
}
//
EFI_STATUS
	LegacyLock (
	VOID
	)
{
	EFI_STATUS Status = EFI_PROTOCOL_ERROR;
	EFI_LEGACY_REGION_PROTOCOL* LegacyRegionProtocol;
	UINT32 Granularity = 0;
	if(BS->LocateProtocol(&gLegacyRegion, NULL, (VOID**) &LegacyRegionProtocol) == EFI_SUCCESS) {
		if (LegacyRegionProtocol != NULL) {
			Status = LegacyRegionProtocol->Lock(LegacyRegionProtocol, 0xF0000, 0xFFFF, &Granularity);
		}
	}
	return Status;
}
//
EFI_STATUS
	WPLowMem (
	VOID
	)
	/*
	Description:
	write protects memory range 0x000F0000->0x000FFFFF
	for AMD/Intel CPU's with on board memory controllers
	*/
{
	UINTN CR4 = 0;
	UINT64 BaseAddress = 0xF0000;
	UINT64 Length = SIZE_4KB * 8;
	EFI_STATUS Status = 0;
	UINT64 CacheAttribute = MTRR_CACHE_WRITE_PROTECTED;
	CPUInfo_t CPUInfo = CPUId();
	BOOLEAN AMD = FALSE;
	EFI_CPU_ARCH_PROTOCOL* CpuProtocol;

	if (!CPUInfo.MTRRSupport) {
		return EFI_UNSUPPORTED;
	}

	if (CompareMem(CPUInfo.Vendor, VID_INTEL, 12) == 0) {
	}
	else if (CompareMem(CPUInfo.Vendor, VID_AMD, 12) == 0) {
		/* AMD 0x1D */
		AMD = TRUE;
		CacheAttribute |= 0x18;
	}
	else {
		return EFI_UNSUPPORTED;
	}
	Status = EFI_SUCCESS;
	BS->LocateProtocol(&gCpuProtocol, NULL, (VOID**) &CpuProtocol);
	/* flush CPU cache */
	if (CpuProtocol != NULL) {
		CpuProtocol->FlushDataCache(CpuProtocol, BaseAddress, Length, EfiCpuFlushTypeWriteBackInvalidate);
	}
	/* try the protocol first */
	if (CpuProtocol != NULL && CpuProtocol->SetMemoryAttributes(CpuProtocol, BaseAddress, Length, EFI_MEMORY_WP) == EFI_SUCCESS) {
		Status = WriteTest(BaseAddress);
	}
	if (Status != EFI_WRITE_PROTECTED) {
		/* protocol failed try alt method */
		if (AMD) {
			MTRRModEnable();
		}
		CR4 = PreMtrrChange();
		Status = ProgramFixedMtrr(CacheAttribute, &BaseAddress, &Length);
		PostMtrrChange(CR4);
		if (AMD) {
			MTRRModDisable();
		}
	}

	BaseAddress = 0xF8000;
	Length = SIZE_4KB * 8;
	Status = EFI_SUCCESS;
	/* flush CPU cache */
	if (CpuProtocol != NULL) {
		CpuProtocol->FlushDataCache(CpuProtocol, BaseAddress, Length, EfiCpuFlushTypeWriteBackInvalidate);
	}
	/* try protocol first */
	if(CpuProtocol != NULL && CpuProtocol->SetMemoryAttributes(CpuProtocol, BaseAddress, Length, EFI_MEMORY_WP) == EFI_SUCCESS) {
		Status = WriteTest(BaseAddress);
	}
	if(Status != EFI_WRITE_PROTECTED) {
		/* protocol failed try alt method */
		if (AMD) {
			MTRRModEnable();
		}
		CR4 = PreMtrrChange();
		Status = ProgramFixedMtrr(CacheAttribute, &BaseAddress, &Length);
		PostMtrrChange(CR4);
		if (AMD) {
			MTRRModDisable();
		}
	}
	return Status;
}
//
EFI_STATUS
	WBLowMem (
	VOID
	)
	/*
	Description:
	unlocks memory range 0x000F0000->0x000FFFFF
	for AMD/Intel CPU's with on board memory controllers
	*/
{
	UINTN CR4 = 0;
	UINT64 BaseAddress = 0xF0000;
	UINT64 Length = SIZE_4KB * 8;
	EFI_STATUS Status = 0;
	UINT64 CacheAttribute = MTRR_CACHE_WRITE_BACK;
	CPUInfo_t CPUInfo = CPUId();
	BOOLEAN AMD = FALSE;
	EFI_CPU_ARCH_PROTOCOL* CpuProtocol;

	if (!CPUInfo.MTRRSupport) {
		return EFI_UNSUPPORTED;
	}

	if (CompareMem(CPUInfo.Vendor, VID_INTEL, 12) == 0) {
	}
	else if (CompareMem(CPUInfo.Vendor, VID_AMD, 12) == 0) {
		/* AMD 0x1E */
		AMD = TRUE;
		CacheAttribute |= 0x18;
	}
	else {
		return EFI_UNSUPPORTED;
	}
	Status = EFI_WRITE_PROTECTED;
	BS->LocateProtocol(&gCpuProtocol, NULL, (VOID**) &CpuProtocol);
	/* try the protocol first */
	if (CpuProtocol != NULL && CpuProtocol->SetMemoryAttributes(CpuProtocol, BaseAddress, Length, EFI_MEMORY_WB) == EFI_SUCCESS) {
		Status = WriteTest(BaseAddress);
	}
	if (Status == EFI_WRITE_PROTECTED) {
		/* protocol failed try alt method */
		if (AMD) {
			MTRRModEnable();
		}
		CR4 = PreMtrrChange();
		Status = ProgramFixedMtrr(CacheAttribute, &BaseAddress, &Length);
		PostMtrrChange(CR4);
		if (AMD) {
			MTRRModDisable();
		}
	}

	BaseAddress = 0xF8000;
	Length = SIZE_4KB * 8;
	Status = EFI_WRITE_PROTECTED;
	/* try protocol first */
	if(CpuProtocol != NULL && CpuProtocol->SetMemoryAttributes(CpuProtocol, BaseAddress, Length, EFI_MEMORY_WB) == EFI_SUCCESS) {
		Status = WriteTest(BaseAddress);
	}
	if(Status == EFI_WRITE_PROTECTED) {
		/* protocol failed try alt method */
		if (AMD) {
			MTRRModEnable();
		}
		CR4 = PreMtrrChange();
		Status = ProgramFixedMtrr(CacheAttribute, &BaseAddress, &Length);
		PostMtrrChange(CR4);
		if (AMD) {
			MTRRModDisable();
		}
	}
	return Status;
}
//
EFI_STATUS
	WriteTest(
	UINT64 Address
	)
{
	VOID * ScanResult = 0;
	UINT8 Test[0x10] = {0};
	ScanResult = ScanMem(Address, Address + (SIZE_4KB * 8), 0x10);
	if(ScanResult == NULL) {
		return EFI_WRITE_PROTECTED;
	}
	SetMem(ScanResult, 0x10, 0xFF);
	if(CompareMem(Test, ScanResult, 0x10) == 0)
	{
		return EFI_WRITE_PROTECTED;
	}
	else
	{
		SetMem(ScanResult, 0x10, 0x00);
		return EFI_SUCCESS;
	}
}
//
EFI_STATUS
	Injector (
	VOID
	)
	/*
	Description:
	injects SLIC/MSDM into acpi tables & injects SLP 1.0 string into low memory
	assumes the memory for the ACPI tables is WP and moves the RSDP, RSDT, XSDT
	SLIC, MSDM into allocated memory modifies RSDP, RSDT, XSDT as needed then 
	updates the System Configuration Table. 
	*/
{
	EFI_STATUS Status = 0;
	/* acpi */
	UINT32 Allocate_Length = 0;
	VOID *ACPI_Table_Buffer = 0;
	RSDP20_t *Low_RSDP_Table = 0;
	UINT8 RSDP_Revision = 0;
	UINT32 RSDP_Length = 0;
	RSDP20_t *New_RSDP_Table = 0;
	RSDTtbl_t *New_RSDT_Table = 0;
	XSDTtbl_t *New_XSDT_Table = 0;
	SLICtbl_t *SLIC_Table = 0;
	RSDP20_t *RSDP_Table = 0;
	RSDTtbl_t *RSDT_Table = 0;
	XSDTtbl_t *XSDT_Table = 0;
	INT32 i = 0;
#if BETA == 1
	DESC_HEADER_t *Low_RSDT_Table = 0;
	DESC_HEADER_t *Low_XSDT_Table = 0;
	DESC_HEADER_t *Next_Table = 0;
	VOID *Next_Table_Buffer = 0;
#endif
#if MSDM_INJECT == 1
	VOID *MSDM_Table_Buffer = 0;
	MSDMtbl_t *MSDM_Table = 0;
#endif
	/* SLP 1.0 */
#if SLP10_INJECT == 1
	VOID *ScanResult = 0;
#endif

	//=========================================================================//
	// get ACPI addresses                                                      //
	//=========================================================================//

#pragma region AcpiAddr

	/* find RSDP in BDA, EBDA */

	Low_RSDP_Table = (RSDP20_t *) FindAcpiRsdPtr();

	/* find RSDP in EFI configuration table */

	Status = GetSystemConfigurationTable (&gEfiAcpi20TableGuid, (VOID **)&RSDP_Table);
	if (EFI_ERROR (Status)) {
		Status = GetSystemConfigurationTable (&gEfiAcpiTableGuid, (VOID **)&RSDP_Table);
	}

	/* set ACPI table variables */

	if (Status == EFI_SUCCESS) {
		RSDP_Revision = RSDP_Table->Revision;
		if (RSDP_Revision == 0) {
			RSDT_Table = (RSDTtbl_t *) RSDP_Table->RSDTAddress;
			RSDP_Length = 0x14;
		}
		else if (RSDP_Revision == 2) {
			RSDT_Table = (RSDTtbl_t *) RSDP_Table->RSDTAddress;
			XSDT_Table = (XSDTtbl_t *) RSDP_Table->XSDTAddress;
			RSDP_Length = RSDP_Table->Length;
		}
		else {
			Status = EFI_UNSUPPORTED;

#if VERBOSE > 0
			Print(L"RSDP revision: %r\r\n", Status);
#endif

		}
	}
	if (EFI_ERROR (Status)) {

#if VERBOSE > 0
		Print(L"ERROR: ACPI tables %r\r\n", Status);
#endif

		return Status;
	}

	/* display ACPI table addresses */

#if VERBOSE > 0
	Print(L"RSDP @ 0x%X\r\nRSDP revision 0x%X\r\nRSDP length 0x%X\r\n", RSDP_Table,  RSDP_Revision, RSDP_Length);
#if VERBOSE == 2
	DumpHex(0, 0, RSDP_Length, RSDP_Table);
#endif
	Print(L"RSDT @ 0x%X\r\n", RSDT_Table);
#if VERBOSE == 2
	DumpHex(0, 0, RSDT_Table->Header.Length, RSDT_Table);
#endif
	if (RSDP_Revision == 2) {
		Print(L"XSDT @ 0x%X\r\n", XSDT_Table);
#if VERBOSE == 2
		DumpHex(0, 0, XSDT_Table->Header.Length, XSDT_Table);
#endif
	}
#endif

#pragma endregion

	//=========================================================================//
	// allocate memory for RSDP, RSDT, XSDT, MSDM                              //
	//=========================================================================//

#pragma region AllocMem

	/* calculate memory needed for revision 1 tables */

	Allocate_Length = RSDP_Length + 4 + RSDT_Table->Header.Length + 8 + sizeof(SLIC) + 4;

#if MSDM_INJECT == 1
	Allocate_Length += 4;
#endif

	/* calculate memory needed for revision 2 tables */

	if (RSDP_Revision == 2) {
		Allocate_Length += XSDT_Table->Header.Length + 12;

#if MSDM_INJECT == 1
		Allocate_Length += 8;
#endif

	}

	/* allocate the memory */

	Status = BS->AllocatePool(EfiACPIReclaimMemory, Allocate_Length, &ACPI_Table_Buffer);
	if (EFI_ERROR (Status)) {
		Print(L"ERROR: allocate memory: %r\r\n", Status);
		BS->FreePool(ACPI_Table_Buffer);
		return Status;
	}

	/* initialize allocated memory to 0 */

	ZeroMem(ACPI_Table_Buffer, Allocate_Length);

#if MSDM_INJECT == 1

	/* allocate memory for MSDM */

	Status = BS->AllocatePool(EfiACPIReclaimMemory, (sizeof(MSDM) + 4), &MSDM_Table_Buffer);
	if (EFI_ERROR (Status)) {
		Print(L"ERROR: allocate memory: %r\r\n", Status);
		BS->FreePool(MSDM_Table_Buffer);
		ContinueKey(0);
		return Status;
	}

	/* initialize allocated memory to 0 */

	ZeroMem(MSDM_Table_Buffer, (sizeof(MSDM) + 4));

#endif

#if VERBOSE > 0
	Print(L"0x%X bytes of memory allocated @ 0x%X\n", Allocate_Length, ACPI_Table_Buffer);
#endif

#pragma endregion

	//=========================================================================//
	// copy RSDP operation                                                     //
	//=========================================================================//

#pragma region CopyRsdp

	/* copy RSDP */

	CopyMem(ACPI_Table_Buffer, RSDP_Table, RSDP_Length);

	/* set RSDP pointer */

	New_RSDP_Table = (RSDP20_t *) ACPI_Table_Buffer;

#if VERBOSE > 0
	Print(L"RSDP copied to 0x%X\r\n", New_RSDP_Table);
#endif

#pragma endregion

	//=========================================================================//
	// copy RSDT operation                                                     //
	//=========================================================================//

#pragma region CopyRsdt

	/* calculate destination pointer */

	New_RSDT_Table = (RSDTtbl_t *) ((UINT64) New_RSDP_Table + RSDP_Length + 4);

	/* copy RSDT */

	CopyMem(New_RSDT_Table, RSDT_Table, RSDT_Table->Header.Length);

#if VERBOSE > 0
	Print(L"RSDT copied to 0x%X\r\n", New_RSDT_Table);
	Print(L"Updating RSDT pointer in RSDP\r\n");
#endif

	/* update RSDT address in RSDP */

	New_RSDP_Table->RSDTAddress = (UINT32) New_RSDT_Table;

#pragma endregion

	//=========================================================================//
	// copy XSDT operation                                                     //
	//=========================================================================//

#pragma region CopyXsdt

	if (RSDP_Revision == 2) {

		/* calculate destination pointer */

#if MSDM_INJECT == 1
		New_XSDT_Table = (XSDTtbl_t *) ((UINT64) New_RSDT_Table + New_RSDT_Table->Header.Length + 8 + 4);
#else
		New_XSDT_Table = (XSDTtbl_t *) ((UINT64) New_RSDT_Table + New_RSDT_Table->Header.Length + 8);
#endif

		/* copy XSDT */

		CopyMem(New_XSDT_Table, XSDT_Table, XSDT_Table->Header.Length);

#if VERBOSE > 0
		Print(L"XSDT copied to 0x%X\r\n", New_XSDT_Table);
		Print(L"Updating XSDT pointer in RSDP\r\n");
#endif

		/* update XSDT address in the RSDP */

		New_RSDP_Table->XSDTAddress = (UINT64) New_XSDT_Table;
	}

#pragma endregion

#if VERBOSE == 2
	Print(L"Press ENTER to continue");
	ContinueKey(30);
	ST->ConOut->ClearScreen(ST->ConOut);
#endif

#if NOENCRYPT == 0
	//=========================================================================//
	// decrypt key                                                             //
	//=========================================================================//

#pragma region DecryptKey

	for(i = 0; i < sizeof(key); i++) {
		key[i] ^= 0xFF;
	}

#pragma endregion

	//=========================================================================//
	// decrypt SLIC                                                            //
	//=========================================================================//

#pragma region DecryptSlic

	for(i = 0; i < sizeof(SLIC); i++) {
		SLIC[i] ^= key[pos];
		pos--;
		if (pos == -1) {
			pos = sizeof(key)-1;
		}
	}

#pragma endregion
#endif

	//=========================================================================//
	// copy SLIC operation                                                     //
	//=========================================================================//

#pragma region CopySlic

	/* calculate destination pointer for revision 1 tables */

	if (RSDP_Revision == 0) {

#if MSDM_INJECT == 1
		SLIC_Table = (SLICtbl_t *) ((UINT64) New_RSDT_Table + New_RSDT_Table->Header.Length + 8 + 4);
#else
		SLIC_Table = (SLICtbl_t *) ((UINT64) New_RSDT_Table + New_RSDT_Table->Header.Length + 8);
#endif

	}

	/* calculate destination pointer for revision 2 tables */

	else if (RSDP_Revision == 2) {

#if MSDM_INJECT == 1
		SLIC_Table = (SLICtbl_t *) ((UINT64) New_XSDT_Table + New_XSDT_Table->Header.Length + 12 + 8);
#else
		SLIC_Table = (SLICtbl_t *) ((UINT64) New_XSDT_Table + New_XSDT_Table->Header.Length + 12);
#endif

	}

	/* copy SLIC */

	CopyMem(SLIC_Table, SLIC, sizeof(SLIC));

#if VERBOSE > 0
	Print(L"SLIC copied to 0x%X\r\n", SLIC_Table);
#if VERBOSE == 2
	DumpHex(0, 0, sizeof(SLIC), SLIC_Table);
#endif
#endif

#pragma endregion

#if VERBOSE == 2
	Print(L"Press ENTER to continue");
	ContinueKey(30);
	ST->ConOut->ClearScreen(ST->ConOut);
#endif

	//=========================================================================//
	// fix table consistency                                                   //
	//=========================================================================//

#pragma region FixConsistency

	CopyMem(New_RSDT_Table->Header.OEMId, SLIC_Table->Header.OEMId, 6);
	CopyMem(New_RSDT_Table->Header.OEMTableId, SLIC_Table->Header.OEMTableId, 8);
	CopyMem(New_RSDP_Table->OemId, SLIC_Table->Header.OEMId, 6);
	if (RSDP_Revision == 2) {
		CopyMem(New_XSDT_Table->Header.OEMId, SLIC_Table->Header.OEMId, 6);
		CopyMem(New_XSDT_Table->Header.OEMTableId, SLIC_Table->Header.OEMTableId, 8);
	}

#pragma endregion

	//=========================================================================//
	// copy & modify & checksum MSDM                                           //
	//=========================================================================//

#pragma region Msdm

#if MSDM_INJECT == 1

	CopyMem(MSDM_Table_Buffer, MSDM, sizeof(MSDM));
	MSDM_Table = (MSDMtbl_t *) MSDM_Table_Buffer;
	CopyMem(MSDM_Table->Header.OEMId, SLIC_Table->Header.OEMId, 6);
	CopyMem(MSDM_Table->Header.OEMTableId, SLIC_Table->Header.OEMTableId, 8);
	MSDM_Table->Header.Checksum = 0;
	MSDM_Table->Header.Checksum = ComputeChecksum((UINT8 *) MSDM_Table, MSDM_Table->Header.Length);

#if VERBOSE > 0
	Print(L"MSDM copied to 0x%X\r\n", MSDM_Table_Buffer);
#if VERBOSE == 2
	DumpHex(0, 0, sizeof(MSDM), MSDM_Table_Buffer);
	Print(L"Press ENTER to continue");
	ContinueKey(30);
	ST->ConOut->ClearScreen(ST->ConOut);
#endif
#endif

#endif

#pragma endregion

	//=========================================================================//
	// copy SLP 1.0 string                                                     //
	//=========================================================================//

#pragma region CopySlpStr

#if SLP10_INJECT == 1

	/* find space for SLP 1.0 string */

	if (SLPLength > 4) {
		ScanResult = ScanMem(ScanStart, ScanEnd, SLPLength);
		if (ScanResult != NULL) {

			/* found space for the SLP 1.0 string, copy it to low memory */
			if (LegacyUnlock() == EFI_SUCCESS) {
				CopyMem(ScanResult, SLPString, SLPLength);
#if VERBOSE > 0
				Print(L"SLP 1.0 copied to 0x%X\r\n", ScanResult);
#if VERBOSE == 2
				DumpHex(0, 0, sizeof(SLPString), ScanResult);
#endif
#endif
			}
			LegacyLock();
		}
	}
#endif

#pragma endregion

	//=========================================================================//
	// add SLIC/MSDM pointer(s) to RSDT                                        //
	//=========================================================================//

#pragma region RsdtPtr

	/* add SLIC pointer to RSDT */

#if VERBOSE > 0
	Print(L"Adding SLIC pointer to RSDT\r\n");
#endif

	New_RSDT_Table->Entry[(New_RSDT_Table->Header.Length - sizeof(DESC_HEADER_t)) >> 2] = (UINT32)SLIC_Table;
	New_RSDT_Table->Header.Length += sizeof(UINT32);

	/* add MSDM pointer to RSDT */

#if MSDM_INJECT == 1

#if VERBOSE > 0
	Print(L"Adding MSDM pointer to RSDT\r\n");
#endif

	New_RSDT_Table->Entry[(New_RSDT_Table->Header.Length - sizeof(DESC_HEADER_t)) >> 2] = (UINT32)MSDM_Table;
	New_RSDT_Table->Header.Length += sizeof(UINT32);

#endif

	/* recalculate RSDT checksum */

#if VERBOSE > 0
	Print(L"Updating RSDT checksum\r\n");
#endif

	New_RSDT_Table->Header.Checksum = 0;
	New_RSDT_Table->Header.Checksum = ComputeChecksum((UINT8 *) New_RSDT_Table, New_RSDT_Table->Header.Length);

#if VERBOSE == 2
	DumpHex(0, 0, New_RSDT_Table->Header.Length, New_RSDT_Table);
#endif

#pragma endregion

	//=========================================================================//
	// add SLIC/MSDM pointer(s) to XSDT                                        //
	//=========================================================================//

#pragma region XsdtPtr

	/* add SLIC pointer to XSDT */

	if (RSDP_Revision == 2) {

#if VERBOSE > 0
		Print(L"Adding SLIC pointer to XSDT\r\n");
#endif

		New_XSDT_Table->Entry[(New_XSDT_Table->Header.Length - sizeof(DESC_HEADER_t)) >> 3] = (UINT64)SLIC_Table;
		New_XSDT_Table->Header.Length += sizeof(UINT64);

		/* add MSDM pointer to XSDT */

#if MSDM_INJECT == 1

#if VERBOSE > 0
		Print(L"Adding MSDM pointer to XSDT\r\n");
#endif

		New_XSDT_Table->Entry[(New_XSDT_Table->Header.Length - sizeof(DESC_HEADER_t)) >> 3] = (UINT64)MSDM_Table;
		New_XSDT_Table->Header.Length += sizeof(UINT64);

#endif

		/* recalculate XSDT checksum */

#if VERBOSE > 0
		Print(L"Updating XSDT checksum\r\n");
#endif

		New_XSDT_Table->Header.Checksum = 0;
		New_XSDT_Table->Header.Checksum = ComputeChecksum((UINT8 *) New_XSDT_Table, New_XSDT_Table->Header.Length);

#if VERBOSE == 2
		DumpHex(0, 0, New_XSDT_Table->Header.Length, New_XSDT_Table);
#endif

	}

#pragma endregion

	//=========================================================================//
	// recalculate RSDP checksum(s)                                            //
	//=========================================================================//

#pragma region RsdpChecksum

#if VERBOSE > 0
	Print(L"Updating RSDP checksum\r\n");
#endif

	New_RSDP_Table->Checksum = 0;
	New_RSDP_Table->Checksum = ComputeChecksum((UINT8 *) New_RSDP_Table, 0x14);

	/* RSDP version 2 has a second "extended" checksum */

	if (RSDP_Revision == 2) {

#if VERBOSE > 0
		Print(L"Updating RSDP extended checksum\r\n");
#endif

		New_RSDP_Table->ExtendedChecksum = 0;
		New_RSDP_Table->ExtendedChecksum = ComputeChecksum((UINT8 *) New_RSDP_Table, RSDP_Length);
	}

#if VERBOSE == 2
	DumpHex(0, 0, RSDP_Length, New_RSDP_Table);
#endif

#pragma endregion

	//=========================================================================//
	// low memory RSDP overwrite                                               //
	//=========================================================================//

#pragma region LowRsdp

#if NOMEM == 0
	if (Low_RSDP_Table != NULL) {
#if VERBOSE > 0
		Print(L"Updating low RSDT\r\n");
#endif
		if (LegacyUnlock() == EFI_SUCCESS) {
			CopyMem(Low_RSDP_Table, New_RSDP_Table, RSDP_Length);
		}
		LegacyLock();
	}
#if VERBOSE > 0
	else {
		Print(L"No low RSDT to update\r\n");
	}
#endif
#endif

#pragma endregion

	//=========================================================================//
	// no RSDP in low memory or overwrite succedded erase the old tables       //
	//=========================================================================//

#pragma region EraseTables

	if ((Low_RSDP_Table == NULL) || (CompareMem(Low_RSDP_Table, New_RSDP_Table, RSDP_Length) == 0)) {
		ZeroMem(RSDP_Table, RSDP_Length);
		ZeroMem(RSDT_Table, RSDT_Table->Header.Length);
		if(RSDP_Revision == 2) {
			ZeroMem(XSDT_Table, XSDT_Table->Header.Length);
		}
	}

#pragma endregion

	//=========================================================================//
	// if an RSDP is in low memory and the overwrite failed, try to move       // 
	// patched RSDT or XSDT back too original position in physical memory,     //
	// move blocking table if necessary                                        //
	// TODO: find DSDT's position in physical memory relative to RSDT and XSDT //
	// this code is obsolete due to the ability to unlock WP memory and modify //
	// the RSDP                                                                //
	//=========================================================================//

#pragma region Beta

	else if ((Low_RSDP_Table != NULL) && (CompareMem(Low_RSDP_Table, New_RSDP_Table, RSDP_Length) != 0)) {

#if BETA == 1

		if (RSDP_Revision == 0) {
			Next_Table = LowTableRSDT(RSDT_Table);
			if (((UINT32) Next_Table - (UINT32) RSDT_Table) <  New_RSDT_Table->Header.Length) {
				/* move next table */
				Status = BS->AllocatePool(EfiACPIReclaimMemory,Next_Table->Length, &Next_Table_Buffer);
				CopyMem(Next_Table_Buffer, Next_Table, Next_Table->Length);
				ZeroMem(Next_Table, Next_Table->Length);
				UpdateRSDT((UINT32) Next_Table, (UINT32) Next_Table_Buffer, New_RSDT_Table);
				UpdateRSDT((UINT32) Next_Table, (UINT32) Next_Table_Buffer, RSDT_Table);
				Next_Table = LowTableRSDT(RSDT_Table);
			}
			if (((UINT32) Next_Table - (UINT32) XSDT_Table) >=  New_XSDT_Table->Header.Length) {
				CopyMem(RSDT_Table, New_RSDT_Table, New_RSDT_Table->Header.Length);
				CopyMem(RSDP_Table->OemId, SLIC_Table->Header.OEMId, 6);
				RSDP_Table->Checksum = 0;
				RSDP_Table->Checksum = ComputeChecksum((UINT8 *) RSDP_Table, 0x14);
				ZeroMem(New_RSDP_Table, RSDP_Length);
				ZeroMem(New_RSDT_Table, New_RSDT_Table->Header.Length);
				New_RSDP_Table = RSDP_Table;
			}

		}
		else if (RSDP_Revision == 2) {
			Low_RSDT_Table = LowTableRSDT(RSDT_Table);
			Low_XSDT_Table = LowTableXSDT(XSDT_Table);
			if ((UINT32) Low_RSDT_Table < (UINT32) Low_XSDT_Table) {
				Next_Table = Low_RSDT_Table;
			}
			else {
				Next_Table = Low_XSDT_Table;
			}
			if (((UINT32) Next_Table - (UINT32) XSDT_Table) <  New_XSDT_Table->Header.Length) {
				/* move next table */
				Status = BS->AllocatePool(EfiACPIReclaimMemory,Next_Table->Length, &Next_Table_Buffer);
				CopyMem(Next_Table_Buffer, Next_Table, Next_Table->Length);
				ZeroMem(Next_Table, Next_Table->Length);
				UpdateRSDT((UINT32) Next_Table, (UINT32) Next_Table_Buffer, RSDT_Table);
				UpdateXSDT((UINT64) Next_Table, (UINT64) Next_Table_Buffer, XSDT_Table);
				UpdateRSDT((UINT32) Next_Table, (UINT32) Next_Table_Buffer, New_RSDT_Table);
				UpdateXSDT((UINT64) Next_Table, (UINT64) Next_Table_Buffer, New_XSDT_Table);
				Low_RSDT_Table = LowTableRSDT(RSDT_Table);
				Low_XSDT_Table = LowTableXSDT(XSDT_Table);
				if ((UINT32) Low_RSDT_Table < (UINT32) Low_XSDT_Table) {
					Next_Table = Low_RSDT_Table;
				}
				else {
					Next_Table = Low_XSDT_Table;
				}
			}
			if (((UINT32) Next_Table - (UINT32) XSDT_Table) >=  New_XSDT_Table->Header.Length) {
				CopyMem(XSDT_Table, New_XSDT_Table, New_XSDT_Table->Header.Length);
				CopyMem(RSDP_Table->OemId, SLIC_Table->Header.OEMId, 6);
				CopyMem(RSDT_Table->Header.OEMId, SLIC_Table->Header.OEMId, 6);
				CopyMem(RSDT_Table->Header.OEMTableId, SLIC_Table->Header.OEMTableId, 8);
				RSDP_Table->Checksum = 0;
				RSDP_Table->Checksum = ComputeChecksum((UINT8 *) RSDP_Table, 0x14);
				RSDP_Table->ExtendedChecksum = 0;
				RSDP_Table->ExtendedChecksum = ComputeChecksum((UINT8 *) RSDP_Table, RSDP_Length);
				ZeroMem(New_RSDP_Table, RSDP_Length);
				ZeroMem(New_RSDT_Table, New_RSDT_Table->Header.Length);
				ZeroMem(New_XSDT_Table, New_XSDT_Table->Header.Length);
				New_RSDP_Table = RSDP_Table;
			}
		}

#endif

	}

#pragma endregion

#if VERBOSE == 2
	Print(L"Press ENTER to continue");
	ContinueKey(30);
	ST->ConOut->ClearScreen(ST->ConOut);
#endif

	//=========================================================================//
	// update configuration table with the RSDP new location                   //
	//=========================================================================//

#pragma region CfgTbl

	/* install ACPI 2.0 EFI configuration table */ 

	Status = BS->InstallConfigurationTable(&gEfiAcpi20TableGuid, New_RSDP_Table);
	if (Status == EFI_SUCCESS) {
		if (GetSystemConfigurationTable (&gEfiAcpi20TableGuid, (VOID **)&RSDP_Table) == EFI_SUCCESS) {

#if VERBOSE > 0
			Print(L"ACPI 2.0 table now points to 0x%X\r\n", RSDP_Table);
#endif

		}
	}
	else {
		Print(L"ERROR: update ACPI 2.0 configuration table\r\n");
	}

	/* install ACPI 1.0 EFI configuration table */

	Status = BS->InstallConfigurationTable(&gEfiAcpiTableGuid, New_RSDP_Table);
	if (Status == EFI_SUCCESS) {
		if (GetSystemConfigurationTable (&gEfiAcpiTableGuid, (VOID **) &RSDP_Table) == EFI_SUCCESS) {

#if VERBOSE > 0
			Print(L"ACPI table now points to 0x%X\r\n", RSDP_Table);
#endif

		}
	}
	else {
		Print(L"ERROR: update ACPI configuration table\r\n");
	}

#if VERBOSE == 2
	DumpHex(0, 0, sizeof(EFI_CONFIGURATION_TABLE) * ST->NumberOfTableEntries, ST->ConfigurationTable);
#endif

#pragma endregion

	/* done return status */

	return Status;
}
//