#ifndef LIBEFI_H
#define LIBEFI_H
//
#include <efi.h>
#include "global.h"
//
// dpath.cpp
//
EFI_DEVICE_PATH *
FileDevicePath (
    IN EFI_HANDLE       Device  OPTIONAL,
    IN CHAR16           *FileName
    );
//
EFI_DEVICE_PATH *
DevicePathFromHandle (
    IN EFI_HANDLE       Handle
    );
//
EFI_DEVICE_PATH *
AppendDevicePath (
    IN EFI_DEVICE_PATH  *Src1,
    IN EFI_DEVICE_PATH  *Src2
    );
//
UINTN
DevicePathInstanceCount (
    IN EFI_DEVICE_PATH      *DevicePath
    );
//
EFI_DEVICE_PATH *
DevicePathInstance (
    IN OUT EFI_DEVICE_PATH  **DevicePath,
    OUT UINTN               *Size
    );
//
UINTN
DevicePathSize (
    IN EFI_DEVICE_PATH  *DevPath
    );
//
EFI_DEVICE_PATH *
DuplicateDevicePath (
    IN EFI_DEVICE_PATH  *DevPath
    );
//
// error.cpp
//
VOID
StatusToString (
    OUT CHAR16          *Buffer,
    IN EFI_STATUS       Status
    );
//
// event.cpp
//
EFI_STATUS
WaitForSingleEvent (
    IN EFI_EVENT        Event,
    IN UINT64           Timeout OPTIONAL
    );
//
// guid.cpp
//
INTN
CompareGuid (
    IN EFI_GUID     *Guid1,
    IN EFI_GUID     *Guid2
    );
//
// math.cpp
//
UINT64
LShiftU64 (
    IN UINT64   Operand,
    IN UINTN    Count
    );
//
UINT64
RShiftU64 (
    IN UINT64   Operand,
    IN UINTN    Count
    );
//
UINT64
MultU64x32 (
    IN UINT64   Multiplicand,
    IN UINTN    Multiplier
    );
//
UINT64
DivU64x32 (
    IN UINT64   Dividend,
    IN UINTN    Divisor,
    OUT UINTN   *Remainder OPTIONAL
    );
//
// mem.cpp
//
VOID
ZeroMem (
    IN VOID     *Buffer,
    IN UINTN     Size
    );
//
VOID
SetMem (
    IN VOID     *Buffer,
    IN UINTN    Size,
    IN UINT8    Value    
    );
//
VOID
CopyMem (
    IN VOID     *Dest,
    IN VOID     *Src,
    IN UINTN    len
    );
//
INTN
CompareMem (
    IN VOID     *Dest,
    IN VOID     *Src,
    IN UINTN    len
    );
//
VOID *
AllocatePool (
    IN UINTN                Size
    );
//
VOID *
AllocateZeroPool (
    IN UINTN                Size
    );
//
VOID *
ScanMem (
	UINTN ScanStart,
	UINTN ScanEnd,
	UINTN Len
	);
//
// print.cpp
//
VOID
DumpHex (
    IN UINTN        Indent,
    IN UINTN        Offset,
    IN UINTN        DataSize,
    IN VOID         *UserData
    );
//
UINTN
Print (
    IN CHAR16   *fmt,
    ...
    );
//
UINTN
SPrint (
    OUT CHAR16  *Str,
    IN UINTN    StrSize,
    IN CHAR16   *fmt,
    ...
    );
//
// str.cpp
//
VOID
StrCpy (
    IN CHAR16   *Dest,
    IN CHAR16   *Src
    );
//
UINTN
StrLen (
    IN CHAR16   *s1
    );
//
UINTN
StrSize (
    IN CHAR16   *s1
    );
//
// memset.cpp
//
void * __cdecl memset (
        void *dst,
        int val,
        size_t count
        );
//
#pragma intrinsic(memset)
//
#endif