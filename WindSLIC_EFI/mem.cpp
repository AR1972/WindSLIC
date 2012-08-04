#include <efi.h>
#include "libefi.h"
//
VOID
ZeroMem (
    IN VOID     *Buffer,
    IN UINTN     Size
    )
{
    INT8        *pt;

    pt = (INT8*)Buffer;
    while (Size--) {
        *(pt++) = 0;
    }
}
//
VOID
SetMem (
    IN VOID     *Buffer,
    IN UINTN    Size,
    IN UINT8    Value    
    )
{
    INT8        *pt;

    pt = (INT8*)Buffer;
    while (Size--) {
        *(pt++) = Value;
    }
}
//
VOID
CopyMem (
    IN VOID     *Dest,
    IN VOID     *Src,
    IN UINTN    len
    )
{
    CHAR8    *d, *s;

    d = (CHAR8*)Dest;
    s = (CHAR8*)Src;
    while (len--) {
        *(d++) = *(s++);
    }
}
//
INTN
CompareMem (
    IN VOID     *Dest,
    IN VOID     *Src,
    IN UINTN    len
    )
{
    CHAR8    *d, *s;

    d = (CHAR8*)Dest;
    s = (CHAR8*)Src;
    while (len--) {
        if (*d != *s) {
            return *d - *s;
        }

        d += 1;
        s += 1;
    }

    return 0;
}
//
VOID *
AllocatePool (
    IN UINTN                Size
    )
{
    EFI_STATUS              Status;
    VOID                    *p;

    Status = BS->AllocatePool (EfiBootServicesData, Size, &p);
    if (EFI_ERROR(Status)) {
        p = NULL;
    }
    return p;
}
//
VOID *
AllocateZeroPool (
    IN UINTN                Size
    )
{
    VOID                    *p;

    p = AllocatePool (Size);
    if (p) {
        ZeroMem (p, Size);
    }

    return p;
}
//
VOID *
ScanMem (
	UINTN ScanStart,
	UINTN ScanEnd,
	UINTN Len
	)
{
	UINTN Address;
	UINTN Count = 0;

	if (Len < 0x10) {
		Len = 0x10;
	}

	for (Address = ScanStart; Address < (ScanEnd-Len); Address += 0x10) {
		Count = 0;
		while (*(UINT8 *) (Address + Count) == 0x0) {
			Count++;
			if (Count == Len) {
				return (VOID *) Address;
			}
		}
	}
	return NULL;
}
//