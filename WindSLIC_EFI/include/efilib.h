#ifndef _EFILIB_INCLUDE_
#define _EFILIB_INCLUDE_

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

    efilib.h

Abstract:

    EFI library functions



Revision History

--*/

#include "efidebug.h"
#include "efipart.h"
#include "efilibplat.h"
#include "link.h"
#include "EfiRtLib.h"
#include "pci22.h"
#include "LibSmbios.h"

//
// Public read-only data in the EFI library
//

extern EFI_SYSTEM_TABLE         *ST;
extern EFI_BOOT_SERVICES        *BS;
extern EFI_RUNTIME_SERVICES     *RT;

extern EFI_GUID DevicePathProtocol;
extern EFI_GUID LoadedImageProtocol;
extern EFI_GUID TextInProtocol;
extern EFI_GUID TextOutProtocol;
extern EFI_GUID BlockIoProtocol;
extern EFI_GUID DiskIoProtocol;
extern EFI_GUID FileSystemProtocol;
extern EFI_GUID LoadFileProtocol;
extern EFI_GUID DeviceIoProtocol;
extern EFI_GUID VariableStoreProtocol;
extern EFI_GUID LegacyBootProtocol;
extern EFI_GUID UnicodeCollationProtocol;
extern EFI_GUID SerialIoProtocol;
extern EFI_GUID VgaClassProtocol;
extern EFI_GUID TextOutSpliterProtocol;
extern EFI_GUID ErrorOutSpliterProtocol;
extern EFI_GUID TextInSpliterProtocol;
extern EFI_GUID SimpleNetworkProtocol;
extern EFI_GUID PxeBaseCodeProtocol;
extern EFI_GUID PxeCallbackProtocol;
extern EFI_GUID NetworkInterfaceIdentifierProtocol;
extern EFI_GUID UiProtocol;
extern EFI_GUID InternalShellProtocol;

extern EFI_GUID DriverBindingProtocol;
extern EFI_GUID BusSpecificDriverOverrideProtocol;
extern EFI_GUID PlatformDriverOverrideProtocol;
extern EFI_GUID PciRootBridgeIoProtocol;
extern EFI_GUID PciIoProtocol;

extern EFI_GUID SimplePointerProtocol ;
extern EFI_GUID BisProtocol; 
extern EFI_GUID ComponentNameProtocol;
extern EFI_GUID DecompressProtocol;
extern EFI_GUID DriverConfigurationProtocol;
extern EFI_GUID DriverDiagnosticsProtocol ;
extern EFI_GUID EbcProtocol;
extern EFI_GUID PlatformDriverOverrideProtocol;
extern EFI_GUID ScsiPassThruProtocol ;
extern EFI_GUID UgaDrawProtocol ;
extern EFI_GUID UgaIoProtocol;
extern EFI_GUID UsbIoProtocol ;
extern EFI_GUID UsbHcProtocol ;
extern EFI_GUID DebugSupportProtocol ; 

extern EFI_GUID EfiGlobalVariable;
extern EFI_GUID GenericFileInfo;
extern EFI_GUID FileSystemInfo;
extern EFI_GUID FileSystemVolumeLabelInfo;
extern EFI_GUID PcAnsiProtocol;
extern EFI_GUID Vt100Protocol;
extern EFI_GUID Vt100PlusProtocol;
extern EFI_GUID VtUtf8Protocol;

extern EFI_GUID NullGuid;
extern EFI_GUID UnknownDevice;

extern EFI_GUID EfiPartTypeSystemPartitionGuid;
extern EFI_GUID EfiPartTypeLegacyMbrGuid;

extern EFI_GUID MpsTableGuid;
extern EFI_GUID AcpiTableGuid;
extern EFI_GUID Acpi20TableGuid;
extern EFI_GUID SMBIOSTableGuid;
extern EFI_GUID SalSystemTableGuid;



//
// EFI Variable strings
//
#define LOAD_OPTION_ACTIVE           0x00000001
#define LOAD_OPTION_FORCE_RECONNECT  0x00000002

#define VarLanguageCodes       L"LangCodes"
#define VarLanguage            L"Lang"
#define VarTimeout             L"Timeout"
#define VarConsoleInp          L"ConIn"
#define VarConsoleOut          L"ConOut"
#define VarErrorOut            L"ErrOut"
#define VarBootOption          L"Boot%04x"
#define VarBootOrder           L"BootOrder"
#define VarBootNext            L"BootNext"
#define VarBootCurrent         L"BootCurrent"
#define VarDriverOption        L"Driver%04x"
#define VarDriverOrder         L"DriverOrder"
#define VarConsoleInpDev       L"ConInDev"
#define VarConsoleOutDev       L"ConOutDev"
#define VarErrorOutDev         L"ErrOutDev"

#define LanguageCodeEnglish    "eng"

extern EFI_DEVICE_PATH *RootDevicePath;
extern EFI_DEVICE_PATH EndDevicePath[];
extern EFI_DEVICE_PATH EndInstanceDevicePath[];
extern EFI_DEVICE_PATH *LegacyDevicePath;

//
// Other public data in the EFI library
//

extern EFI_MEMORY_TYPE PoolAllocationType;

//
// STATIC - Name is internal to the module
// INTERNAL - Name is internal to the component (i.e., directory)
// BOOTSERVCE - Name of a boot service function
//

#define STATIC
#define INTERNAL
#define BOOTSERVICE

//
// Prototypes
//

VOID
InitializeLib (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
    );

void 
EfiCheckVersion(
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
    );

void
EfiGetVersion(
    IN UINTN               *MajorVersion,
    IN UINTN               *MinorVersion
);

BOOLEAN
IsEfi102(
);

BOOLEAN
IsEfi110(
);


VOID
InitializeUnicodeSupport (
    CHAR8 *LangCode
    );

VOID
EFIDebugVariable (
    VOID
    );

VOID
SetCrc (
    IN OUT EFI_TABLE_HEADER *Hdr
    );

VOID
SetCrcAltSize (
    IN UINTN                 Size,
    IN OUT EFI_TABLE_HEADER *Hdr
    );

BOOLEAN
CheckCrc (
    IN UINTN                 MaxSize,
    IN OUT EFI_TABLE_HEADER *Hdr
    );

BOOLEAN
CheckCrcAltSize (
    IN UINTN                 MaxSize,
    IN UINTN                 Size,
    IN OUT EFI_TABLE_HEADER *Hdr
    );

UINT32
CalculateCrc (
    UINT8 *pt,
    UINTN Size
    );

VOID
ZeroMem (
    IN VOID     *Buffer,
    IN UINTN     Size
    );

VOID
SetMem (
    IN VOID     *Buffer,
    IN UINTN    Size,
    IN UINT8    Value    
    );

VOID
CopyMem (
    IN VOID     *Dest,
    IN VOID     *Src,
    IN UINTN    len
    );

INTN
CompareMem (
    IN VOID     *Dest,
    IN VOID     *Src,
    IN UINTN    len
    );

INTN
StrCmp (
    IN CHAR16   *s1,
    IN CHAR16   *s2
    );

INTN
StrnCmp (
    IN CHAR16   *s1,
    IN CHAR16   *s2,
    IN UINTN    len
    );

INTN
StriCmp (
    IN CHAR16   *s1,
    IN CHAR16   *s2
    );

VOID
StrLwr (
    IN CHAR16   *Str
    );

VOID
StrUpr (
    IN CHAR16   *Str
    );

VOID
StrCpy (
    IN CHAR16   *Dest,
    IN CHAR16    *Src
    );

VOID
StrCat (
    IN CHAR16   *Dest,
    IN CHAR16   *Src
    );

UINTN
StrLen (
    IN CHAR16   *s1
    );

UINTN
StrSize (
    IN CHAR16   *s1
    );

CHAR16 *
StrDuplicate (
    IN CHAR16   *Src
    );

UINTN
strlena (
    IN CHAR8    *s1
    );
    
UINTN
strcmpa (
    IN CHAR8    *s1,
    IN CHAR8    *s2
    );

UINTN
strncmpa (
    IN CHAR8    *s1,
    IN CHAR8    *s2,
    IN UINTN    len
    );

UINTN
xtoi (
    CHAR16      *str
    );

UINTN
Atoi (
    CHAR16  *str
    );

BOOLEAN 
MetaMatch (
    IN CHAR16   *String,
    IN CHAR16   *Pattern
    );

BOOLEAN 
MetaiMatch (
    IN CHAR16   *String,
    IN CHAR16   *Pattern
    );

UINT64
LShiftU64 (
    IN UINT64   Operand,
    IN UINTN    Count
    );

UINT64
RShiftU64 (
    IN UINT64   Operand,
    IN UINTN    Count
    );

UINT64
MultU64x32 (
    IN UINT64   Multiplicand,
    IN UINTN    Multiplier
    );

UINT64
DivU64x32 (
    IN UINT64   Dividend,
    IN UINTN    Divisor,
    OUT UINTN   *Remainder OPTIONAL
    );

VOID
InitializeLock (
    IN OUT FLOCK    *Lock,
    IN EFI_TPL  Priority
    );

VOID
AcquireLock (
    IN FLOCK    *Lock
    );

VOID
ReleaseLock (
    IN FLOCK    *Lock
    );


INTN
CompareGuid(
    IN EFI_GUID     *Guid1,
    IN EFI_GUID     *Guid2
    );

VOID *
AllocatePool (
    IN UINTN     Size
    );

VOID *
AllocateZeroPool (
    IN UINTN     Size
    );

VOID *
ReallocatePool (
    IN VOID                 *OldPool,
    IN UINTN                OldSize,
    IN UINTN                NewSize
    );

VOID
FreePool (
    IN VOID     *p
    );


VOID
Output (
    IN CHAR16   *Str
    );

VOID
Input (
    IN CHAR16   *Prompt OPTIONAL,
    OUT CHAR16  *InStr,
    IN UINTN    StrLen
    );

VOID
IInput (
    IN SIMPLE_TEXT_OUTPUT_INTERFACE     *ConOut,
    IN SIMPLE_INPUT_INTERFACE           *ConIn,
    IN CHAR16                           *Prompt OPTIONAL,
    OUT CHAR16                          *InStr,
    IN UINTN                            StrLen
    );

UINTN
Print (
    IN CHAR16   *fmt,
    ...
    );

UINTN
SPrint (
    OUT CHAR16  *Str,
    IN UINTN    StrSize,
    IN CHAR16   *fmt,
    ...
    );

CHAR16 *
PoolPrint (
    IN CHAR16           *fmt,
    ...
    );

typedef struct {
    CHAR16      *str;
    UINTN       len;
    UINTN       maxlen;
} POOL_PRINT;

CHAR16 *
CatPrint (
    IN OUT POOL_PRINT   *Str,
    IN CHAR16           *fmt,
    ...
    );

UINTN
PrintAt (
    IN UINTN    Column,
    IN UINTN    Row,
    IN CHAR16   *fmt,
    ...
    );

UINTN
IPrint (
    IN SIMPLE_TEXT_OUTPUT_INTERFACE    *Out,
    IN CHAR16                          *fmt,
    ...
    );

UINTN
IPrintAt (
    IN SIMPLE_TEXT_OUTPUT_INTERFACE     *Out,
    IN UINTN                            Column,
    IN UINTN                            Row,
    IN CHAR16                           *fmt,
    ...
    );

UINTN
APrint (
    IN CHAR8    *fmt,
    ...
    );

VOID
ValueToHex (
    IN CHAR16   *Buffer,
    IN UINT64   v
    );

VOID
ValueToString (
    IN CHAR16   *Buffer,
    IN BOOLEAN  Comma,
    IN INT64    v
    );

VOID
TimeToString (
    OUT CHAR16      *Buffer,
    IN EFI_TIME     *Time
    );

VOID
GuidToString (
    OUT CHAR16      *Buffer,
    IN EFI_GUID     *Guid
    );

VOID
StatusToString (
    OUT CHAR16      *Buffer,
    EFI_STATUS      Status
    );

VOID
DumpHex (
    IN UINTN        Indent,
    IN UINTN        Offset,
    IN UINTN        DataSize,
    IN VOID         *UserData
    );

BOOLEAN
GrowBuffer(
    IN OUT EFI_STATUS   *Status,
    IN OUT VOID         **Buffer,
    IN UINTN            BufferSize
    );

EFI_MEMORY_DESCRIPTOR *
LibMemoryMap (
    OUT UINTN               *NoEntries,
    OUT UINTN               *MapKey,
    OUT UINTN               *DescriptorSize,
    OUT UINT32              *DescriptorVersion
    );

VOID *
LibGetVariable (
    IN CHAR16               *Name,
    IN EFI_GUID             *VendorGuid
    );

VOID *
LibGetVariableAndSize (
    IN CHAR16               *Name,
    IN EFI_GUID             *VendorGuid,
    OUT UINTN               *VarSize
    );

EFI_STATUS
LibDeleteVariable (
    IN CHAR16   *VarName,
    IN EFI_GUID *VarGuid
    );

EFI_STATUS
LibInsertToTailOfBootOrder (
    IN  UINT16  BootOption,
    IN  BOOLEAN OnlyInsertIfEmpty
    );

EFI_STATUS
LibLocateProtocol (
    IN  EFI_GUID    *ProtocolGuid,
    OUT VOID        **Interface
    );

EFI_STATUS
LibLocateHandle (
    IN EFI_LOCATE_SEARCH_TYPE   SearchType,
    IN EFI_GUID                 *Protocol OPTIONAL,
    IN VOID                     *SearchKey OPTIONAL,
    IN OUT UINTN                *NoHandles,
    OUT EFI_HANDLE              **Buffer
    );

EFI_STATUS
LibLocateHandleByDiskSignature (
    IN UINT8                        MBRType,
    IN UINT8                        SignatureType,
    IN VOID                         *Signature,
    IN OUT UINTN                    *NoHandles,
    OUT EFI_HANDLE                  **Buffer
    );

EFI_STATUS
LibInstallProtocolInterfaces (
    IN OUT EFI_HANDLE       *Handle,
    ...
    );

VOID
LibUninstallProtocolInterfaces (
    IN EFI_HANDLE           Handle,
    ...
    );

EFI_STATUS
LibReinstallProtocolInterfaces (
    IN OUT EFI_HANDLE           *Handle,
    ...
    );

EFI_EVENT
LibCreateProtocolNotifyEvent (
    IN EFI_GUID             *ProtocolGuid,
    IN EFI_TPL              NotifyTpl,
    IN EFI_EVENT_NOTIFY     NotifyFunction,
    IN VOID                 *NotifyContext,
    OUT VOID                *Registration
    );

EFI_STATUS
WaitForSingleEvent (
    IN EFI_EVENT        Event,
    IN UINT64           Timeout OPTIONAL
    );

VOID
WaitForEventWithTimeout (
    IN  EFI_EVENT       Event,
    IN  UINTN           Timeout,
    IN  UINTN           Row,
    IN  UINTN           Column,
    IN  CHAR16          *String,
    IN  EFI_INPUT_KEY   TimeoutKey,
    OUT EFI_INPUT_KEY   *Key
    );

EFI_FILE_HANDLE
LibOpenRoot (
    IN EFI_HANDLE           DeviceHandle
    );

EFI_FILE_INFO *
LibFileInfo (
    IN EFI_FILE_HANDLE      FHand
    );

EFI_FILE_SYSTEM_INFO *
LibFileSystemInfo (
    IN EFI_FILE_HANDLE      FHand
    );

EFI_FILE_SYSTEM_VOLUME_LABEL_INFO *
LibFileSystemVolumeLabelInfo (
    IN EFI_FILE_HANDLE      FHand
    );

BOOLEAN
ValidMBR(
    IN  MASTER_BOOT_RECORD  *Mbr,
    IN  EFI_BLOCK_IO        *BlkIo
    );

BOOLEAN
LibMatchDevicePaths (
    IN  EFI_DEVICE_PATH *Multi,
    IN  EFI_DEVICE_PATH *Single
    );

EFI_DEVICE_PATH *
LibDuplicateDevicePathInstance (
    IN EFI_DEVICE_PATH  *DevPath
    );

EFI_DEVICE_PATH *
DevicePathFromHandle (
    IN EFI_HANDLE           Handle
    );

EFI_DEVICE_PATH *
DevicePathInstance (
    IN OUT EFI_DEVICE_PATH  **DevicePath,
    OUT UINTN               *Size
    );

UINTN
DevicePathInstanceCount (
    IN EFI_DEVICE_PATH      *DevicePath
    );

EFI_DEVICE_PATH *
AppendDevicePath (
    IN EFI_DEVICE_PATH      *Src1,
    IN EFI_DEVICE_PATH      *Src2
    );

EFI_DEVICE_PATH *
AppendDevicePathNode (
    IN EFI_DEVICE_PATH      *Src1,
    IN EFI_DEVICE_PATH      *Src2
    );

EFI_DEVICE_PATH*
AppendDevicePathInstance (
    IN EFI_DEVICE_PATH  *Src,
    IN EFI_DEVICE_PATH  *Instance
    );

EFI_DEVICE_PATH *
FileDevicePath (
    IN EFI_HANDLE           Device  OPTIONAL,
    IN CHAR16               *FileName
    );

UINTN
DevicePathSize (
    IN EFI_DEVICE_PATH      *DevPath
    );

EFI_DEVICE_PATH *
DuplicateDevicePath (
    IN EFI_DEVICE_PATH      *DevPath
    );

EFI_DEVICE_PATH *
UnpackDevicePath (
    IN EFI_DEVICE_PATH      *DevPath
    );

EFI_STATUS
LibDevicePathToInterface (
    IN EFI_GUID             *Protocol,
    IN EFI_DEVICE_PATH      *FilePath,
    OUT VOID                **Interface
    );

CHAR16 *
DevicePathToStr (
    EFI_DEVICE_PATH         *DevPath
    );

//
// BugBug: I need my own include files
//
typedef struct {
    UINT8   Register;
    UINT8   Function;
    UINT8   Device;
    UINT8   Bus;
    UINT32  Reserved;
} EFI_ADDRESS;

typedef union {
    UINT64          Address;
    EFI_ADDRESS     EfiAddress;
} EFI_PCI_ADDRESS_UNION;


EFI_STATUS
PciFindDeviceClass (
    IN  OUT EFI_PCI_ADDRESS_UNION   *Address,
    IN      UINT8                   BaseClass,
    IN      UINT8                   SubClass
    );

EFI_STATUS
PciFindDevice (
    IN  OUT EFI_PCI_ADDRESS_UNION   *DeviceAddress,
    IN      UINT16                  VendorId,
    IN      UINT16                  DeviceId,
    IN OUT  PCI_TYPE00              *Pci
    );

//
// SIMPLE_READ_FILE object used to access files
//

typedef VOID        *SIMPLE_READ_FILE;

EFI_STATUS
OpenSimpleReadFile (
    IN BOOLEAN                  BootPolicy,
    IN VOID                     *SourceBuffer   OPTIONAL,
    IN UINTN                    SourceSize,
    IN OUT EFI_DEVICE_PATH      **FilePath,
    OUT EFI_HANDLE              *DeviceHandle,    
    OUT SIMPLE_READ_FILE        *SimpleReadHandle
    );

EFI_STATUS
ReadSimpleReadFile (
    IN SIMPLE_READ_FILE     SimpleReadHandle,
    IN UINTN                Offset,
    IN OUT UINTN            *ReadSize,
    OUT VOID                *Buffer
    );


VOID
CloseSimpleReadFile (
    IN SIMPLE_READ_FILE     SimpleReadHandle
    );

VOID
InitializeGuid (
    VOID
    );

UINT8
DecimaltoBCD(
    IN  UINT8 DecValue
    );

UINT8
BCDtoDecimal(
    IN  UINT8 BcdValue
    );

EFI_STATUS
LibGetSystemConfigurationTable(
    IN EFI_GUID *TableGuid,
    IN OUT VOID **Table
    );

BOOLEAN
LibIsValidTextGraphics (
    IN  CHAR16  Graphic,   
    OUT CHAR8   *PcAnsi,    OPTIONAL
    OUT CHAR8   *Ascii      OPTIONAL
    );

BOOLEAN
IsValidAscii (
    IN  CHAR16  Ascii
    );

BOOLEAN
IsValidEfiCntlChar (
    IN  CHAR16  c
    );

CHAR16 *
LibGetUiString (
    IN  EFI_HANDLE      Handle,
    IN  UI_STRING_TYPE  StringType,
    IN  ISO_639_2       *LangCode,
    IN  BOOLEAN         ReturnDevicePathStrOnMismatch
    );

#define EFI_HANDLE_TYPE_UNKNOWN                      0x000
#define EFI_HANDLE_TYPE_IMAGE_HANDLE                 0x001
#define EFI_HANDLE_TYPE_DRIVER_BINDING_HANDLE        0x002
#define EFI_HANDLE_TYPE_DEVICE_DRIVER                0x004
#define EFI_HANDLE_TYPE_BUS_DRIVER                   0x008
#define EFI_HANDLE_TYPE_DRIVER_CONFIGURATION_HANDLE  0x010
#define EFI_HANDLE_TYPE_DRIVER_DIAGNOSTICS_HANDLE    0x020
#define EFI_HANDLE_TYPE_COMPONENT_NAME_HANDLE        0x040
#define EFI_HANDLE_TYPE_DEVICE_HANDLE                0x080
#define EFI_HANDLE_TYPE_PARENT_HANDLE                0x100
#define EFI_HANDLE_TYPE_CONTROLLER_HANDLE            0x200
#define EFI_HANDLE_TYPE_CHILD_HANDLE                 0x400


#ifndef EFI_APP_102
EFI_STATUS
LibScanHandleDatabase (
  EFI_HANDLE  DriverBindingHandle,       OPTIONAL    
  UINT32      *DriverBindingHandleIndex, OPTIONAL
  EFI_HANDLE  ControllerHandle,          OPTIONAL
  UINT32      *ControllerHandleIndex,    OPTIONAL
  UINTN       *HandleCount,
  EFI_HANDLE  **HandleBuffer,
  UINT32      **HandleType
  );

EFI_STATUS
LibGetManagingDriverBindingHandles (
  EFI_HANDLE  ControllerHandle,
  UINTN       *DriverBindingHandleCount,
  EFI_HANDLE  **DriverBindingHandleBuffer
  );

EFI_STATUS
LibGetParentControllerHandles (
  EFI_HANDLE  ControllerHandle,
  UINTN       *ParentControllerHandleCount,
  EFI_HANDLE  **ParentControllerHandleBuffer
  );

EFI_STATUS
LibGetManagedChildControllerHandles (
  EFI_HANDLE  DriverBindingHandle,
  EFI_HANDLE  ControllerHandle,
  UINTN       *ChildControllerHandleCount,
  EFI_HANDLE  **ChildControllerHandleBuffer
  );

EFI_STATUS
LibGetManagedControllerHandles (
  EFI_HANDLE  DriverBindingHandle,
  UINTN       *ControllerHandleCount,
  EFI_HANDLE  **ControllerHandleBuffer
  );

EFI_STATUS
LibGetChildControllerHandles (
  EFI_HANDLE  ControllerHandle,
  UINTN       *HandleCount,
  EFI_HANDLE  **HandleBuffer
  );

#endif

CHAR8*
LibGetSmbiosString (
    IN  SMBIOS_STRUCTURE_POINTER    *Smbios,
    IN  UINT16                      StringNumber
    );

EFI_STATUS
LibGetSmbiosSystemGuidAndSerialNumber (
    IN  EFI_GUID    *SystemGuid,
    OUT CHAR8       **SystemSerialNumber
    );
    
VOID *
LibFindAcpiRsdPtr (
    VOID
    );

VOID *
LibFindSMBIOSPtr (
    VOID
    );

VOID *
LibFindMPSPtr (
    VOID
    );

EFI_STATUS
InitializeGlobalIoDevice (
        IN  EFI_DEVICE_PATH             *DevicePath,
        IN  EFI_GUID                    *Protocol,
        IN  CHAR8                       *ErrorStr,
        OUT EFI_DEVICE_IO_INTERFACE     **GlobalIoFncs 
        );

UINT32 
ReadPort (
        IN  EFI_DEVICE_IO_INTERFACE     *GlobalIoFncs, 
        IN  EFI_IO_WIDTH                Width,
        IN  UINTN                       Port
        );

UINT32 
WritePort (
        IN  EFI_DEVICE_IO_INTERFACE     *GlobalIoFncs, 
        IN  EFI_IO_WIDTH                Width,
        IN  UINTN                       Port,
        IN  UINTN                       Data
        );

UINT32 
ReadPciConfig (
        IN  EFI_DEVICE_IO_INTERFACE     *GlobalIoFncs, 
        IN  EFI_IO_WIDTH                Width,
        IN  UINTN                       Port
        );

UINT32 
WritePciConfig (
        IN  EFI_DEVICE_IO_INTERFACE     *GlobalIoFncs, 
        IN  EFI_IO_WIDTH                Width,
        IN  UINTN                       Port,
        IN  UINTN                       Data
        );

extern EFI_DEVICE_IO_INTERFACE  *GlobalIoFncs;

#define outp(_Port, _DataByte)  (UINT8)WritePort(GlobalIoFncs,  IO_UINT8,  (UINTN)_Port, (UINTN)_DataByte)
#define inp(_Port)              (UINT8)ReadPort(GlobalIoFncs,   IO_UINT8,  (UINTN)_Port)
#define outpw(_Port, _DataByte) (UINT16)WritePort(GlobalIoFncs, IO_UINT16, (UINTN)_Port, (UINTN)_DataByte)
#define inpw(_Port)             (UINT16)ReadPort(GlobalIoFncs,  IO_UINT16, (UINTN)_Port)
#define outpd(_Port, _DataByte) (UINT32)WritePort(GlobalIoFncs, IO_UINT32, (UINTN)_Port, (UINTN)_DataByte)
#define inpd(_Port)             (UINT32)ReadPort(GlobalIoFncs,  IO_UINT32, (UINTN)_Port)

#define writepci8(_Addr, _DataByte)  (UINT8)WritePciConfig(GlobalIoFncs,  IO_UINT8,  (UINTN)_Addr, (UINTN)_DataByte)
#define readpci8(_Addr)              (UINT8)ReadPciConfig(GlobalIoFncs,   IO_UINT8,  (UINTN)_Addr)
#define writepci16(_Addr, _DataByte) (UINT16)WritePciConfig(GlobalIoFncs, IO_UINT16, (UINTN)_Addr, (UINTN)_DataByte)
#define readpci16(_Addr)             (UINT16)ReadPciConfig(GlobalIoFncs,  IO_UINT16, (UINTN)_Addr)
#define writepci32(_Addr, _DataByte) (UINT32)WritePciConfig(GlobalIoFncs, IO_UINT32, (UINTN)_Addr, (UINTN)_DataByte)
#define readpci32(_Addr)             (UINT32)ReadPciConfig(GlobalIoFncs,  IO_UINT32, (UINTN)_Addr)

#define Pause()             WaitForSingleEvent (ST->ConIn->WaitForKey, 0)
#define Port80(_PostCode)   GlobalIoFncs->Io.Write (GlobalIoFncs, IO_UINT16, (UINT64)0x80, 1, &(_PostCode))

// EFI_APP_MULTIMODAL
#if defined(EFI_APP_MULTIMODAL)

EFI_TPL 
BOOTSERVICE 
MultimodalRaiseTPL (
    IN EFI_TPL      NewTpl
    );

VOID
BOOTSERVICE 
MultimodalRestoreTPL(
    IN EFI_TPL      OldTpl
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalAllocatePages(
    IN EFI_ALLOCATE_TYPE            Type,
    IN EFI_MEMORY_TYPE              MemoryType,
    IN UINTN                        NoPages,
    OUT EFI_PHYSICAL_ADDRESS        *Memory
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalFreePages(
    IN EFI_PHYSICAL_ADDRESS         Memory,
    IN UINTN                        NoPages
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalGetMemoryMap(
    IN OUT UINTN                    *MemoryMapSize,
    IN OUT EFI_MEMORY_DESCRIPTOR    *MemoryMap,
    OUT UINTN                       *MapKey,
    OUT UINTN                       *DescriptorSize,
    OUT UINT32                      *DescriptorVersion
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalAllocatePool(
    IN EFI_MEMORY_TYPE              PoolType,
    IN UINTN                        Size,
    OUT VOID                        **Buffer
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalFreePool(
    IN VOID                         *Buffer
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalCreateEvent(
    IN UINT32                       Type,
    IN EFI_TPL                      NotifyTpl,
    IN EFI_EVENT_NOTIFY             NotifyFunction,
    IN VOID                         *NotifyContext,
    OUT EFI_EVENT                   *Event
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalSetTimer(
    IN EFI_EVENT                Event,
    IN EFI_TIMER_DELAY          Type,
    IN UINT64                   TriggerTime
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalWaitForEvent(
    IN UINTN                    NumberOfEvents,
    IN EFI_EVENT                *Event,
    OUT UINTN                   *Index
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalSignalEvent(
    IN EFI_EVENT                Event
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalCloseEvent(
    IN EFI_EVENT                Event
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalCheckEvent(
    IN EFI_EVENT                Event
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalInstallProtocolInterface(
    IN OUT EFI_HANDLE           *Handle,
    IN EFI_GUID                 *Protocol,
    IN EFI_INTERFACE_TYPE       InterfaceType,
    IN VOID                     *Interface
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalReinstallProtocolInterface(
    IN EFI_HANDLE               Handle,
    IN EFI_GUID                 *Protocol,
    IN VOID                     *OldInterface,
    IN VOID                     *NewInterface
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalUninstallProtocolInterface(
    IN EFI_HANDLE               Handle,
    IN EFI_GUID                 *Protocol,
    IN VOID                     *Interface
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalHandleProtocol(
    IN EFI_HANDLE               Handle,
    IN EFI_GUID                 *Protocol,
    OUT VOID                    **Interface
    );

EFI_STATUS 
BOOTSERVICE 
MultimodalPCHandleProtocol(
    IN EFI_HANDLE               Handle,
    IN EFI_GUID                 *Protocol,
    OUT VOID                    **Interface
    );


EFI_STATUS  
BOOTSERVICE 
MultimodalRegisterProtocolNotify(
    IN EFI_GUID                 *Protocol,
    IN EFI_EVENT                Event,
    OUT VOID                    **Registration
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalLocateHandle(
    IN EFI_LOCATE_SEARCH_TYPE   SearchType,
    IN EFI_GUID                 *Protocol OPTIONAL,
    IN VOID                     *SearchKey OPTIONAL,
    IN OUT UINTN                *BufferSize,
    OUT EFI_HANDLE              *Buffer
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalLocateDevicePath(
    IN EFI_GUID                 *Protocol,
    IN OUT EFI_DEVICE_PATH      **DevicePath,
    OUT EFI_HANDLE              *Device
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalInstallConfigurationTable(
    IN EFI_GUID                 *Guid,
    IN VOID                     *Table
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalReserved(
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalLoadImage(
    IN BOOLEAN                      BootPolicy,
    IN EFI_HANDLE                   ParentImageHandle,
    IN EFI_DEVICE_PATH              *FilePath,
    IN VOID                         *SourceBuffer   OPTIONAL,
    IN UINTN                        SourceSize,
    OUT EFI_HANDLE                  *ImageHandle
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalStartImage(
    IN EFI_HANDLE                   ImageHandle,
    OUT UINTN                       *ExitDataSize,
    OUT CHAR16                      **ExitData  OPTIONAL
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalExit(
    IN EFI_HANDLE                   ImageHandle,
    IN EFI_STATUS                   ExitStatus,
    IN UINTN                        ExitDataSize,
    IN CHAR16                       *ExitData OPTIONAL
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalUnloadImage(
    IN EFI_HANDLE                   ImageHandle
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalExitBootServices(
    IN EFI_HANDLE                   ImageHandle,
    IN UINTN                        MapKey
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalStall(
    IN UINTN                    Microseconds
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalGetNextMonotonicCount(
    OUT UINT64                  *Count
    );


EFI_STATUS 
BOOTSERVICE 
MultimodalSetWatchdogTimer(
    IN UINTN                    Timeout,
    IN UINT64                   WatchdogCode,
    IN UINTN                    DataSize,
    IN CHAR16                   *WatchdogData OPTIONAL
    );

EFI_STATUS 
BOOTSERVICE 
MultimodalConnectController (
  IN  EFI_HANDLE                ControllerHandle,
  IN  EFI_HANDLE                *DriverImageHandle    OPTIONAL,
  IN  EFI_DEVICE_PATH           *RemainingDevicePath  OPTIONAL,
  IN  BOOLEAN                   Recursive
  );


EFI_STATUS 
BOOTSERVICE
MultimodalDisconnectController (
  IN  EFI_HANDLE  ControllerHandle,
  IN  EFI_HANDLE  DriverImageHandle, OPTIONAL
  IN  EFI_HANDLE  ChildHandle        OPTIONAL
  );


EFI_STATUS
BOOTSERVICE 
MultimodalOpenProtocol (
  IN  EFI_HANDLE                UserHandle,
  IN  EFI_GUID                  *Protocol,
  OUT VOID                      **Interface,
  IN  EFI_HANDLE                ImageHandle,
  IN  EFI_HANDLE                ControllerHandle,
  IN  UINT32                    Attributes
  );


EFI_STATUS
BOOTSERVICE 
MultimodalCloseProtocol (
  IN  EFI_HANDLE                UserHandle,
  IN  EFI_GUID                  *Protocol,
  IN  EFI_HANDLE                ImageHandle,
  IN  EFI_HANDLE                ControllerHandle  OPTIONAL
  );


EFI_STATUS
BOOTSERVICE 
MultimodalOpenProtocolInformation (
  IN  EFI_HANDLE                          UserHandle,
  IN  EFI_GUID                            *Protocol,
  IN  EFI_OPEN_PROTOCOL_INFORMATION_ENTRY **EntryBuffer,
  OUT UINTN                               *EntryCount
  );


EFI_STATUS
BOOTSERVICE 
MultimodalProtocolsPerHandle (
  IN EFI_HANDLE       UserHandle,
  OUT EFI_GUID        ***ProtocolBuffer,
  OUT UINTN           *ProtocolBufferCount
  );


EFI_STATUS
BOOTSERVICE 
MultimodalLocateHandleBuffer (
  IN EFI_LOCATE_SEARCH_TYPE       SearchType,
  IN EFI_GUID                     *Protocol OPTIONAL,
  IN VOID                         *SearchKey OPTIONAL,
  IN OUT UINTN                    *NumberHandles,
  OUT EFI_HANDLE                  **Buffer
  );


EFI_STATUS
BOOTSERVICE 
MultimodalLocateProtocol (
  EFI_GUID  *Protocol,
  VOID      *Registration, OPTIONAL
  VOID      **Interface
  );


EFI_STATUS
BOOTSERVICE 
MultimodalInstallMultipleProtocolInterfaces (
  IN OUT EFI_HANDLE           *Handle,
  ...
  );


EFI_STATUS
BOOTSERVICE 
MultimodalUninstallMultipleProtocolInterfaces (
  IN EFI_HANDLE           Handle,
  ...
  );


EFI_STATUS
BOOTSERVICE
MultimodalCalculateCrc32 (
    VOID   *pt,
    UINTN  Size,
    UINT32 *Crc
    );


VOID
BOOTSERVICE 
MultimodalSetMem (
  IN VOID   *Buffer,
  IN UINTN  Size,
  IN UINT8  Value    
  );


VOID
BOOTSERVICE 
MultimodalCopyMem (
  VOID *Dest,
  VOID *Src,
  UINTN Length
  );



#endif  //EFI_APP_MULTIMODAL

#endif  //File head
