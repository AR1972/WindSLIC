#ifndef MTRR_H
#define MTRR_H
//
#include "base.h"
//
#define  MTRR_CACHE_UNCACHEABLE      0
#define  MTRR_CACHE_WRITE_COMBINING  1
#define  MTRR_CACHE_WRITE_THROUGH    4
#define  MTRR_CACHE_WRITE_PROTECTED  5
#define  MTRR_CACHE_WRITE_BACK       6
#define  MTRR_CACHE_INVALID_TYPE     7
//
#define	VID_INTEL "GenuineIntel"
#define	VID_AMD "AuthenticAMD"
//
typedef struct {
	UINT32 Vendor[3];
	UINT8 SteppingID;
	UINT8 Model;
	UINT8 Family;
	UINT8 ProcessorType;
	UINT8 ExtendedModel;
	UINT8 ExtendedFamily;
	BOOLEAN MTRRSupport;
} CPUInfo_t;
//
VOID 
ClearWP (
	VOID
	);
//
VOID
SetWP (
	VOID
	);
//
VOID
DisableCache (
	VOID
	);
//
VOID
EnableCache (
	VOID
	);
//
VOID
CpuFlushTlb (
	VOID
	);
//
UINTN
PreMtrrChange (
	VOID
	);
//
VOID
PostMtrrChange (
	UINTN Cr4
	);
//
//
UINT64
WriteMsr (
	UINT32 Index,
	UINT64 Value
	);
//
UINT64
ReadMsr (
	IN UINT32 Index
	);
//
EFI_STATUS
ProgramFixedMtrr (
	IN UINT64 MemoryCacheType,
	IN OUT UINT64 *Base,
	IN OUT UINT64 *Length
	);
//
CPUInfo_t
CPUId (
	VOID
	);
//
VOID
MTRRModDisable (
	VOID
	);
//
VOID
MTRRModEnable (
	VOID
	);
//
#endif