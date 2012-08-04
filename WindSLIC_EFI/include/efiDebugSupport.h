/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  DebugSupport.h

Abstract:

  DebugSupport protocol and supporting definitions as defined in the EFI 1.1
  specification.
  
  The DebugSupport protocol is used by source level debuggers to abstract the
  processor and handle context save and restore operations.
 
--*/

#ifndef _DEBUG_SUPPORT_H_
#define _DEBUG_SUPPORT_H_

//
// Debug Support protocol {2755590C-6F3C-42FA-9EA4-A3BA543CDA25}
//

#define EFI_DEBUG_SUPPORT_PROTOCOL_GUID \
  { 0x2755590C, 0x6F3C, 0x42FA, 0x9E, 0xA4, 0xA3, 0xBA, 0x54, 0x3C, 0xDA, 0x25 }

//
// Debug Support definitions
//

typedef INTN                               EFI_EXCEPTION_TYPE;

//
//  IA-32 processor exception types
//
#define   EXCEPT_IA32_DIVIDE_ERROR         0
#define   EXCEPT_IA32_DEBUG                1
#define   EXCEPT_IA32_NMI                  2
#define   EXCEPT_IA32_BREAKPOINT           3
#define   EXCEPT_IA32_OVERFLOW             4
#define   EXCEPT_IA32_BOUND                5
#define   EXCEPT_IA32_INVALID_OPCODE       6
#define   EXCEPT_IA32_DOUBLE_FAULT         8
#define   EXCEPT_IA32_INVALID_TSS         10
#define   EXCEPT_IA32_SEG_NOT_PRESENT     11
#define   EXCEPT_IA32_STACK_FAULT         12
#define   EXCEPT_IA32_GP_FAULT            13
#define   EXCEPT_IA32_PAGE_FAULT          14
#define   EXCEPT_IA32_FP_ERROR            16
#define   EXCEPT_IA32_ALIGNMENT_CHECK     17
#define   EXCEPT_IA32_MACHINE_CHECK       18
#define   EXCEPT_IA32_SIMD                19

//
// FXSAVE_STATE
// FP / MMX / XMM registers (see fxrstor instruction definition)
//

#ifdef EFI32
//
//  IA-32 fx context definition
//
typedef
struct {
  UINT16              Fcw;
  UINT16              Fsw;
  UINT16              Ftw;
  UINT16              Opcode;
  UINT32              Eip;
  UINT16              Cs;
  UINT16              Reserved1;
  UINT32              DataOffset;
  UINT16              Ds;
  UINT8               Reserved2[10];
  UINT8               St0Mm0[10], Reserved3[6];
  UINT8               St0Mm1[10], Reserved4[6];
  UINT8               St0Mm2[10], Reserved5[6];
  UINT8               St0Mm3[10], Reserved6[6];
  UINT8               St0Mm4[10], Reserved7[6];
  UINT8               St0Mm5[10], Reserved8[6];
  UINT8               St0Mm6[10], Reserved9[6];
  UINT8               St0Mm7[10], Reserved10[6];
  UINT8               Reserved11[22 * 16];
} EFI_FX_SAVE_STATE;
#endif

#ifdef EFIX64
//
//  X64 fx context definition
//
typedef
struct {
  UINT16              Fcw;
  UINT16              Fsw;
  UINT16              Ftw;
  UINT16              Opcode;
  UINT64              Eip;
  UINT16              Cs;
  UINT16              Reserved1;
  UINT32              DataOffset;
  UINT16              Ds;
  UINT8               Reserved2[10];
  UINT8               St0Mm0[10], Reserved3[6];
  UINT8               St0Mm1[10], Reserved4[6];
  UINT8               St0Mm2[10], Reserved5[6];
  UINT8               St0Mm3[10], Reserved6[6];
  UINT8               St0Mm4[10], Reserved7[6];
  UINT8               St0Mm5[10], Reserved8[6];
  UINT8               St0Mm6[10], Reserved9[6];
  UINT8               St0Mm7[10], Reserved10[6];
  UINT8               Reserved11[22 * 16];
} EFI_FX_SAVE_STATE;
#endif

//
// container for other architectures
// 
#if !defined(EFI32) && !defined(EFIX64)
typedef
struct {
  UINT8 nouse;
} EFI_FX_SAVE_STATE;
#endif

//
//  IA-32 system context definition
//
typedef
struct {
  UINT32              ExceptionData;
  EFI_FX_SAVE_STATE   FxSaveState;
  UINT32              Dr0, Dr1, Dr2, Dr3, Dr6, Dr7;
  UINT32              Cr0, Cr1 /* Reserved */, Cr2, Cr3, Cr4;
  UINT32              Eflags;
  UINT32              Ldtr, Tr;
  UINT32              Gdtr[2], Idtr[2];
  UINT32              Eip;
  UINT32              Gs, Fs, Es, Ds, Cs, Ss;
  UINT32              Edi, Esi, Ebp, Esp, Ebx, Edx, Ecx, Eax;
} EFI_SYSTEM_CONTEXT_IA32;

//
//  X64 system context definition
//
typedef
struct {
  UINT32              ExceptionData;
  EFI_FX_SAVE_STATE   FxSaveState;
  UINT64              Dr0, Dr1, Dr2, Dr3, Dr6, Dr7;
  UINT64              Cr0, Cr1, Cr2, Cr3, Cr4, Cr8;
  UINT32              Eflags;
  UINT64              Ldtr[4], Tr[4];
  UINT64              Gdtr[2], Idtr[2];                       /* More than needed         */
  INT64               Rip;
  UINT32              Gs, Fs, Es, Ds, Cs, Ss;                 /* Though some are ignored  */
  UINT64              Rdi, Rsi, Rbp, Rsp, Rbx, Rdx, Rcx, Rax;
  UINT64              R8, R9, R10, R11, R12, R13, R14, R15;          
} EFI_SYSTEM_CONTEXT_X64;

//
//  IPF processor exception types
//
#define    EXCEPT_IPF_VHTP_TRANSLATION               0
#define    EXCEPT_IPF_INSTRUCTION_TLB                1
#define    EXCEPT_IPF_DATA_TLB                       2
#define    EXCEPT_IPF_ALT_INSTRUCTION_TLB            3
#define    EXCEPT_IPF_ALT_DATA_TLB                   4
#define    EXCEPT_IPF_DATA_NESTED_TLB                5
#define    EXCEPT_IPF_INSTRUCTION_KEY_MISSED         6
#define    EXCEPT_IPF_DATA_KEY_MISSED                7
#define    EXCEPT_IPF_DIRTY_BIT                      8
#define    EXCEPT_IPF_INSTRUCTION_ACCESS_BIT         9
#define    EXCEPT_IPF_DATA_ACCESS_BIT               10
#define    EXCEPT_IPF_BREAKPOINT                    11
#define    EXCEPT_IPF_EXTERNAL_INTERRUPT            12
// 13 - 19 reserved
#define    EXCEPT_IPF_PAGE_NOT_PRESENT              20
#define    EXCEPT_IPF_KEY_PERMISSION                21
#define    EXCEPT_IPF_INSTRUCTION_ACCESS_RIGHTS     22
#define    EXCEPT_IPF_DATA_ACCESS_RIGHTS            23
#define    EXCEPT_IPF_GENERAL_EXCEPTION             24
#define    EXCEPT_IPF_DISABLED_FP_REGISTER          25
#define    EXCEPT_IPF_NAT_CONSUMPTION               26
#define    EXCEPT_IPF_SPECULATION                   27
// 28 reserved
#define    EXCEPT_IPF_DEBUG                         29
#define    EXCEPT_IPF_UNALIGNED_REFERENCE           30
#define    EXCEPT_IPF_UNSUPPORTED_DATA_REFERENCE    31
#define    EXCEPT_IPF_FP_FAULT                      32
#define    EXCEPT_IPF_FP_TRAP                       33
#define    EXCEPT_IPF_LOWER_PRIVILEGE_TRANSFER_TRAP 34
#define    EXCEPT_IPF_TAKEN_BRANCH                  35
#define    EXCEPT_IPF_SINGLE_STEP                   36
// 37 - 44 reserved
#define    EXCEPT_IPF_IA32_EXCEPTION                45
#define    EXCEPT_IPF_IA32_INTERCEPT                46
#define    EXCEPT_IPF_IA32_INTERRUPT                47


//
//  IPF processor context definition
//
typedef
struct {
//
// The first reserved field is necessary to preserve alignment for the correct
// bits in UNAT and to insure F2 is 16 byte aligned..
//
  UINT64          Reserved;
  UINT64          R1;
  UINT64          R2;
  UINT64          R3;
  UINT64          R4;
  UINT64          R5;
  UINT64          R6;
  UINT64          R7;
  UINT64          R8;
  UINT64          R9;
  UINT64          R10;
  UINT64          R11;
  UINT64          R12;
  UINT64          R13;
  UINT64          R14;
  UINT64          R15;
  UINT64          R16;
  UINT64          R17;
  UINT64          R18;
  UINT64          R19;
  UINT64          R20;
  UINT64          R21;
  UINT64          R22;
  UINT64          R23;
  UINT64          R24;
  UINT64          R25;
  UINT64          R26;
  UINT64          R27;
  UINT64          R28;
  UINT64          R29;
  UINT64          R30;
  UINT64          R31;

  UINT64          F2[2];
  UINT64          F3[2];
  UINT64          F4[2];
  UINT64          F5[2];
  UINT64          F6[2];
  UINT64          F7[2];
  UINT64          F8[2];
  UINT64          F9[2];
  UINT64          F10[2];
  UINT64          F11[2];
  UINT64          F12[2];
  UINT64          F13[2];
  UINT64          F14[2];
  UINT64          F15[2];
  UINT64          F16[2];
  UINT64          F17[2];
  UINT64          F18[2];
  UINT64          F19[2];
  UINT64          F20[2];
  UINT64          F21[2];
  UINT64          F22[2];
  UINT64          F23[2];
  UINT64          F24[2];
  UINT64          F25[2];
  UINT64          F26[2];
  UINT64          F27[2];
  UINT64          F28[2];
  UINT64          F29[2];
  UINT64          F30[2];
  UINT64          F31[2];

  UINT64          Pr;

  UINT64          B0;
  UINT64          B1;
  UINT64          B2;
  UINT64          B3;
  UINT64          B4;
  UINT64          B5;
  UINT64          B6;
  UINT64          B7;

//
// application registers
//
  UINT64          ArRsc;
  UINT64          ArBsp;
  UINT64          ArBspstore;
  UINT64          ArRnat;

  UINT64          ArFcr;

  UINT64          ArEflag;
  UINT64          ArCsd;
  UINT64          ArSsd;
  UINT64          ArCflg;
  UINT64          ArFsr;
  UINT64          ArFir;
  UINT64          ArFdr;

  UINT64          ArCcv;

  UINT64          ArUnat;

  UINT64          ArFpsr;

  UINT64          ArPfs;
  UINT64          ArLc;
  UINT64          ArEc;

//
// control registers
//
  UINT64          CrDcr;
  UINT64          CrItm;
  UINT64          CrIva;
  UINT64          CrPta;
  UINT64          CrIpsr;
  UINT64          CrIsr;
  UINT64          CrIip;
  UINT64          CrIfa;
  UINT64          CrItir;
  UINT64          CrIipa;
  UINT64          CrIfs;
  UINT64          CrIim;
  UINT64          CrIha;

//
// debug registers
//
  UINT64          Dbr0;
  UINT64          Dbr1;
  UINT64          Dbr2;
  UINT64          Dbr3;
  UINT64          Dbr4;
  UINT64          Dbr5;
  UINT64          Dbr6;
  UINT64          Dbr7;

  UINT64          Ibr0;
  UINT64          Ibr1;
  UINT64          Ibr2;
  UINT64          Ibr3;
  UINT64          Ibr4;
  UINT64          Ibr5;
  UINT64          Ibr6;
  UINT64          Ibr7;

//
// virtual registers - nat bits for R1-R31
//
  UINT64          IntNat;

} EFI_SYSTEM_CONTEXT_IPF;


//
//  EBC processor exception types
//
#define EXCEPT_EBC_UNDEFINED                  0
#define EXCEPT_EBC_DIVIDE_ERROR               1
#define EXCEPT_EBC_DEBUG                      2
#define EXCEPT_EBC_BREAKPOINT                 3
#define EXCEPT_EBC_OVERFLOW                   4
#define EXCEPT_EBC_INVALID_OPCODE             5   // opcode out of range
#define EXCEPT_EBC_STACK_FAULT                6
#define EXCEPT_EBC_ALIGNMENT_CHECK            7
#define EXCEPT_EBC_INSTRUCTION_ENCODING       8   // malformed instruction
#define EXCEPT_EBC_BAD_BREAK                  9   // BREAK 0 or undefined BREAK
#define EXCEPT_EBC_STEP                       10  // to support debug stepping

//
// For coding convenience, define the maximum valid EBC exception.
//
#define MAX_EBC_EXCEPTION                     EXCEPT_EBC_STEP

//
//  EBC processor context definition
//
typedef
struct {
  UINT64        R0, R1, R2, R3, R4, R5, R6, R7;
  UINT64        Flags;
  UINT64        ControlFlags;
  UINT64        Ip;
} EFI_SYSTEM_CONTEXT_EBC;

//
// Universal EFI_SYSTEM_CONTEXT definition
//
typedef
union {
  EFI_SYSTEM_CONTEXT_EBC                  *SystemContextEbc;
  EFI_SYSTEM_CONTEXT_IA32                 *SystemContextIa32;
  EFI_SYSTEM_CONTEXT_IPF                  *SystemContextIpf;
  EFI_SYSTEM_CONTEXT_X64                  *SystemContextX64;
} EFI_SYSTEM_CONTEXT;

//
// DebugSupport callback function prototypes
//
typedef
VOID
(* EFI_EXCEPTION_CALLBACK) (
  IN     EFI_EXCEPTION_TYPE               ExceptionType,
  IN OUT EFI_SYSTEM_CONTEXT               SystemContext
  );

typedef
VOID
(* EFI_PERIODIC_CALLBACK) (
  IN OUT EFI_SYSTEM_CONTEXT               SystemContext
  );

//
// Machine type definition
//
typedef
enum {
  IsaIa32 = EFI_IMAGE_MACHINE_IA32,
  IsaIpf  = EFI_IMAGE_MACHINE_IA64,
  IsaEbc  = EFI_IMAGE_MACHINE_EBC,
  IsaX64  = EFI_IMAGE_MACHINE_X64
} EFI_INSTRUCTION_SET_ARCHITECTURE;

EFI_FORWARD_DECLARATION (EFI_DEBUG_SUPPORT_PROTOCOL);

//
// DebugSupport member function definitions
//
typedef
EFI_STATUS
(EFIAPI *EFI_GET_MAXIMUM_PROCESSOR_INDEX) (
  IN EFI_DEBUG_SUPPORT_PROTOCOL          *This,
  OUT UINTN                              *MaxProcessorIndex
  );

typedef
EFI_STATUS
(EFIAPI *EFI_REGISTER_PERIODIC_CALLBACK) (
  IN EFI_DEBUG_SUPPORT_PROTOCOL          *This,
  IN UINTN                               ProcessorIndex,
  IN EFI_PERIODIC_CALLBACK               PeriodicCallback
  );

typedef
EFI_STATUS
(EFIAPI *EFI_REGISTER_EXCEPTION_CALLBACK) (
  IN EFI_DEBUG_SUPPORT_PROTOCOL          *This,
  IN UINTN                               ProcessorIndex,
  IN EFI_EXCEPTION_CALLBACK              ExceptionCallback,
  IN EFI_EXCEPTION_TYPE                  ExceptionType
  );

typedef
EFI_STATUS
(EFIAPI *EFI_INVALIDATE_INSTRUCTION_CACHE) (
  IN EFI_DEBUG_SUPPORT_PROTOCOL          *This,
  IN UINTN                               ProcessorIndex,
  IN VOID                                *Start,
  IN UINT64                              Length
  );

//
// DebugSupport protocol definition
//
typedef struct _EFI_DEBUG_SUPPORT_PROTOCOL {
  EFI_INSTRUCTION_SET_ARCHITECTURE        Isa;
  EFI_GET_MAXIMUM_PROCESSOR_INDEX         GetMaximumProcessorIndex;
  EFI_REGISTER_PERIODIC_CALLBACK          RegisterPeriodicCallback;
  EFI_REGISTER_EXCEPTION_CALLBACK         RegisterExceptionCallback;
  EFI_INVALIDATE_INSTRUCTION_CACHE        InvalidateInstructionCache;
} EFI_DEBUG_SUPPORT_PROTOCOL;

extern EFI_GUID DebugSupportProtocol;

#endif /* _DEBUG_SUPPORT_H_ */

