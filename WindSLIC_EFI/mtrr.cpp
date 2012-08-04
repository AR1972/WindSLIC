#include <intrin.h>
#include <efi.h>
#include "base.h"
#include "libmtrr.h"
#include "libefi.h"
//
#define AMD_SYSCFG 0xc0010010
#define AMD_MTRRFIXRANGE_DRAM_ENABLE 0x00040000 /* MtrrFixDramEn bit    */
#define AMD_MTRRFIXRANGE_DRAM_MODIFY 0x00080000 /* MtrrFixDramModEn bit */
//
typedef struct {
	UINT32  Msr;
	UINT32  BaseAddress;
	UINT32  Length;
} FIXED_MTRR;
//
#define  MTRR_LIB_IA32_MTRR_DEF_TYPE 0x2FF
#define  MTRR_NUMBER_OF_FIXED_MTRR      11
//
#define  MTRR_LIB_IA32_MTRR_CAP                      0x0FE
#define  MTRR_LIB_IA32_MTRR_CAP_VCNT_MASK            0x0FF
#define  MTRR_LIB_IA32_MTRR_FIX64K_00000             0x250
#define  MTRR_LIB_IA32_MTRR_FIX16K_80000             0x258
#define  MTRR_LIB_IA32_MTRR_FIX16K_A0000             0x259
#define  MTRR_LIB_IA32_MTRR_FIX4K_C0000              0x268
#define  MTRR_LIB_IA32_MTRR_FIX4K_C8000              0x269
#define  MTRR_LIB_IA32_MTRR_FIX4K_D0000              0x26A
#define  MTRR_LIB_IA32_MTRR_FIX4K_D8000              0x26B
#define  MTRR_LIB_IA32_MTRR_FIX4K_E0000              0x26C
#define  MTRR_LIB_IA32_MTRR_FIX4K_E8000              0x26D
#define  MTRR_LIB_IA32_MTRR_FIX4K_F0000              0x26E
#define  MTRR_LIB_IA32_MTRR_FIX4K_F8000              0x26F
#define  MTRR_LIB_IA32_VARIABLE_MTRR_BASE            0x200
static
FIXED_MTRR    MtrrLibFixedMtrrTable[] = {
	{
		MTRR_LIB_IA32_MTRR_FIX64K_00000,
			0,
			SIZE_64KB
	},
	{
		MTRR_LIB_IA32_MTRR_FIX16K_80000,
			0x80000,
			SIZE_16KB
	},
	{
		MTRR_LIB_IA32_MTRR_FIX16K_A0000,
			0xA0000,
			SIZE_16KB
	},
	{
		MTRR_LIB_IA32_MTRR_FIX4K_C0000,
			0xC0000,
			SIZE_4KB
	},
	{
		MTRR_LIB_IA32_MTRR_FIX4K_C8000,
			0xC8000,
			SIZE_4KB
	},
	{
		MTRR_LIB_IA32_MTRR_FIX4K_D0000,
			0xD0000,
			SIZE_4KB
	},
	{
		MTRR_LIB_IA32_MTRR_FIX4K_D8000,
			0xD8000,
			SIZE_4KB
	},
	{
		MTRR_LIB_IA32_MTRR_FIX4K_E0000,
			0xE0000,
			SIZE_4KB
	},
	{
		MTRR_LIB_IA32_MTRR_FIX4K_E8000,
			0xE8000,
			SIZE_4KB
	},
	{
		MTRR_LIB_IA32_MTRR_FIX4K_F0000,
			0xF0000,
			SIZE_4KB
	},
	{
		MTRR_LIB_IA32_MTRR_FIX4K_F8000,
			0xF8000,
			SIZE_4KB
	},
};
//
#define  ASSERT(a)
//
UINT64
MsrBitFieldWrite (
	IN UINT32 Index,
	IN UINTN StartBit,
	IN UINTN EndBit,
	IN UINT64 Value
	);
//
UINT64
BitFieldWrite (
	IN UINT64 Operand,
	IN UINTN StartBit,
	IN UINTN EndBit,
	IN UINT64 Value
	);
//
UINT64
BitFieldAndThenOr (
	IN UINT64 Operand,
	IN UINTN StartBit,
	IN UINTN EndBit,
	IN UINT64 AndData,
	IN UINT64 OrData
	);
//
UINT64
BitFieldAnd(
	IN UINT64 Operand,
	IN UINTN StartBit,
	IN UINTN EndBit,
	IN UINT64 AndData
	);
//
UINT64
BitFieldOr(
	IN UINT64 Operand,
	IN UINTN StartBit,
	IN UINTN EndBit,
	IN UINT64 OrData
	);
//
VOID
ClearWP (
	VOID
	)
{
	__writecr0(__readcr0() & (~BIT16));
}
//
VOID
SetWP (
	VOID
	)
{
	__writecr0(__readcr0() | BIT16);
}
//
VOID
DisableCache (
	VOID
	)
{
	UINT64 Value;
	Value = __readcr0();
	Value |= BIT30;
	Value &= ~BIT29;
	__writecr0(Value);
	__wbinvd();
}
//
VOID
EnableCache (
	VOID
	)
{
	UINT64 Value;
	__wbinvd();
	Value = __readcr0();
	Value &= ~BIT30;
	Value &= ~BIT29;
	__writecr0(Value);
}
//
VOID
CpuFlushTlb (
	VOID
	)
{
	__writecr3(__readcr3());
}
//
UINT64
MsrBitFieldWrite (
	IN UINT32 Index,
	IN UINTN StartBit,
	IN UINTN EndBit,
	IN UINT64 Value
	)
{
	return WriteMsr (Index, BitFieldWrite (ReadMsr (Index), StartBit, EndBit, Value));
}
//
UINT64
WriteMsr (
	UINT32 Index,
	UINT64 Value
	)
{
	__writemsr(Index, Value);
	return Value;
}
//
UINT64
ReadMsr (
	IN UINT32 Index
	)
{
	return __readmsr(Index);
}
//
UINT64
BitFieldWrite (
	IN UINT64 Operand,
	IN UINTN StartBit,
	IN UINTN EndBit,
	IN UINT64 Value
	)
{
	ASSERT (EndBit < 64);
	ASSERT (StartBit <= EndBit);
	return BitFieldAndThenOr (Operand, StartBit, EndBit, 0, Value);
}
//
UINT64
BitFieldAndThenOr (
	IN UINT64 Operand,
	IN UINTN StartBit,
	IN UINTN EndBit,
	IN UINT64 AndData,
	IN UINT64 OrData
	)
{
	ASSERT (EndBit < 64);
	ASSERT (StartBit <= EndBit);
	return BitFieldOr (BitFieldAnd (Operand, StartBit, EndBit, AndData), StartBit, EndBit, OrData);
}
//
UINT64
BitFieldOr (
	IN UINT64 Operand,
	IN UINTN StartBit,
	IN UINTN EndBit,
	IN UINT64 OrData
	)
{
	UINT64  Value1;
	UINT64  Value2;

	ASSERT (EndBit < 64);
	ASSERT (StartBit <= EndBit);

	Value1 = LShiftU64 (OrData, StartBit);
	Value2 = LShiftU64 ((UINT64) - 2, EndBit);

	return Operand | (Value1 & ~Value2);
}
//
UINT64
BitFieldAnd (
	IN UINT64 Operand,
	IN UINTN StartBit,
	IN UINTN EndBit,
	IN UINT64 AndData
	)
{
	UINT64  Value1;
	UINT64  Value2;

	ASSERT (EndBit < 64);
	ASSERT (StartBit <= EndBit);

	Value1 = LShiftU64 (~AndData, StartBit);
	Value2 = LShiftU64 ((UINT64)-2, EndBit);

	return Operand & ~(Value1 & ~Value2);
}
//
/**
Programs fixed MTRRs registers.

@param  MemoryCacheType  The memory type to set.
@param  Base             The base address of memory range.
@param  Length           The length of memory range.

@retval RETURN_SUCCESS      The cache type was updated successfully
@retval RETURN_UNSUPPORTED  The requested range or cache type was invalid
for the fixed MTRRs.

**/
EFI_STATUS
ProgramFixedMtrr (
	IN     UINT64     MemoryCacheType,
	IN OUT UINT64     *Base,
	IN OUT UINT64     *Length
	)
{
	UINT32  MsrNum;
	UINT32  ByteShift;
	UINT64  TempQword;
	UINT64  OrMask;
	UINT64  ClearMask;

	TempQword = 0;
	OrMask    = 0;
	ClearMask = 0;

	for (MsrNum = 0; MsrNum < MTRR_NUMBER_OF_FIXED_MTRR; MsrNum++) {
		if ((*Base >= MtrrLibFixedMtrrTable[MsrNum].BaseAddress) &&
			(*Base <
			(
			MtrrLibFixedMtrrTable[MsrNum].BaseAddress +
			(8 * MtrrLibFixedMtrrTable[MsrNum].Length)
			)
			)
			) {
				break;
		}
	}

	if (MsrNum == MTRR_NUMBER_OF_FIXED_MTRR) {
		return EFI_UNSUPPORTED;
	}
	//
	// We found the fixed MTRR to be programmed
	//
	for (ByteShift = 0; ByteShift < 8; ByteShift++) {
		if (*Base ==
			(
			MtrrLibFixedMtrrTable[MsrNum].BaseAddress +
			(ByteShift * MtrrLibFixedMtrrTable[MsrNum].Length)
			)
			) {
				break;
		}
	}

	if (ByteShift == 8) {
		return EFI_UNSUPPORTED;
	}

	for (
		;
		((ByteShift < 8) && (*Length >= MtrrLibFixedMtrrTable[MsrNum].Length));
	ByteShift++
		) {
			OrMask |= LShiftU64 ((UINT64) MemoryCacheType, (UINT32) (ByteShift * 8));
			ClearMask |= LShiftU64 ((UINT64) 0xFF, (UINT32) (ByteShift * 8));
			*Length -= MtrrLibFixedMtrrTable[MsrNum].Length;
			*Base += MtrrLibFixedMtrrTable[MsrNum].Length;
	}

	if (ByteShift < 8 && (*Length != 0)) {
		return EFI_UNSUPPORTED;
	}

	TempQword =
		(ReadMsr (MtrrLibFixedMtrrTable[MsrNum].Msr) & ~ClearMask) | OrMask;
	WriteMsr (MtrrLibFixedMtrrTable[MsrNum].Msr, TempQword);
	return EFI_SUCCESS;
}
//
UINTN
PreMtrrChange (
	VOID
	)
{
	UINTN Value;
	//
	// Enter no fill cache mode, CD=1(Bit30), NW=0 (Bit29)
	//
	DisableCache();
	//
	// Save original CR4 value and clear PGE flag (Bit 7)
	//
	Value = __readcr4();
	__writecr4(Value & (~BIT7));
	//
	// Flush all TLBs
	//
	CpuFlushTlb();
	//
	// Disable Mtrrs
	//
	MsrBitFieldWrite (MTRR_LIB_IA32_MTRR_DEF_TYPE, 10, 11, 0);
	//
	// Return original CR4 value
	//
	return Value;
}
//
VOID
PostMtrrChange (
	UINTN Cr4
	)
{
	//
	// Enable Cache MTRR
	//
	MsrBitFieldWrite (MTRR_LIB_IA32_MTRR_DEF_TYPE, 10, 11, 3);
	//
	// Flush all TLBs 
	//
	CpuFlushTlb();
	//
	// Enable Normal Mode caching CD=NW=0, CD(Bit30), NW(Bit29)
	//
	EnableCache();
	//
	// Restore original CR4 value
	//
	__writecr4(Cr4);
}
//
CPUInfo_t
CPUId (
	VOID
	)
{
	int Buffer[4] = {-1};
	CPUInfo_t CpuInfo;
	__cpuid(Buffer, 0);
	ZeroMem(&CpuInfo, 12);
	CpuInfo.Vendor[0] = Buffer[1];
	CpuInfo.Vendor[1] = Buffer[3];
	CpuInfo.Vendor[2] = Buffer[2];
	__cpuid(Buffer, 1);
	CpuInfo.SteppingID = Buffer[0] & 0xF;
	CpuInfo.Model = (Buffer[0] >> 4) & 0xF;
	CpuInfo.Family = (Buffer[0] >> 8) & 0xF;
	CpuInfo.ProcessorType = (Buffer[0] >> 12) & 0x3;
	CpuInfo.ExtendedModel = (Buffer[0] >> 16) & 0xF;
	CpuInfo.ExtendedFamily = (Buffer[0] >> 20) & 0xFF;
	CpuInfo.MTRRSupport = Buffer[3] & BIT12 || FALSE;
	return CpuInfo;
}
//
VOID
MTRRModEnable (
	VOID
	)
{
	WriteMsr(AMD_SYSCFG, ReadMsr(AMD_SYSCFG) | AMD_MTRRFIXRANGE_DRAM_MODIFY);
	WriteMsr(AMD_SYSCFG, ReadMsr(AMD_SYSCFG) | AMD_MTRRFIXRANGE_DRAM_ENABLE);
}
//
VOID
MTRRModDisable (
	VOID
	)
{
	WriteMsr(AMD_SYSCFG, ReadMsr(AMD_SYSCFG) & (~AMD_MTRRFIXRANGE_DRAM_MODIFY));
	WriteMsr(AMD_SYSCFG, ReadMsr(AMD_SYSCFG) | AMD_MTRRFIXRANGE_DRAM_ENABLE);
}
//