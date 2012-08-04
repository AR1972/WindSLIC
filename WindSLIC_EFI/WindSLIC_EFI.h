#ifndef WINDSLIC_H
#define WINDSLIC_H
//
/* This program is free software. It comes without any warranty, to
* the extent permitted by applicable law. You can redistribute it
* and/or modify it under the terms of the Do What The Fuck You Want
* To Public License, Version 2, as published by Sam Hocevar. See
* http://sam.zoy.org/wtfpl/COPYING for more details. */
//
BOOLEAN
ContinueKey (
	UINT64 Timeout OPTIONAL
	);
//
UINT8
ComputeChecksum (
	UINT8 *Buffer,
	UINT32 Length
	);
//
EFI_STATUS
GetSystemConfigurationTable (
	IN EFI_GUID *TableGuid,
	IN OUT VOID **Table
	);
//
VOID *
FindAcpiRsdPtr (
	VOID
	);
//
BOOLEAN
ScanTableInRsdt (
	RSDTtbl_t *Rsdt,
	UINT32 Signature,
	VOID **FoundTable
	);
//
BOOLEAN
ScanTableInXsdt (
	XSDTtbl_t *Xsdt,
	UINT32 Signature,
	VOID **FoundTable
	);
//
DESC_HEADER_t *
LowTableXsdt (
	XSDTtbl_t *Xsdt
	);
//
DESC_HEADER_t *
LowTableRsdt (
	RSDTtbl_t *Rsdt
	);
//
BOOLEAN
UpdateXsdt (
	UINT64 Old_Address,
	UINT64 New_Address,
	XSDTtbl_t *Xsdt
	);
//
BOOLEAN
UpdateRsdt (
	UINT32 Old_Address,
	UINT32 New_Address,
	RSDTtbl_t *Rsdt
	);
//
EFI_STATUS
WPLowMem (
	VOID
	);
//
EFI_STATUS
WBLowMem (
	VOID
	);
//
EFI_STATUS
	Injector(
	VOID
	);
//
EFI_STATUS
	WriteTest(
	UINT64 Address
	);
//
EFI_STATUS
	LegacyUnlock (
	VOID
	);
//
EFI_STATUS
	LegacyLock (
	VOID
	);
//
#endif