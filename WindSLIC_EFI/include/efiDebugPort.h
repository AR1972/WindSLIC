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

    DebugPort.h

Abstract:

--*/


#ifndef _DEBUG_PORT_H_
#define _DEBUG_PORT_H_

//#include "EfiApi.h"

//
// DebugPortIo protocol {EBA4E8D2-3858-41EC-A281-2647BA9660D0}
//
#define EFI_DEBUGPORT_PROTOCOL_GUID \
  { 0xEBA4E8D2, 0x3858, 0x41EC, 0xA2, 0x81, 0x26, 0x47, 0xBA, 0x96, 0x60, 0xD0 }

extern EFI_GUID  DebugPortProtocol;

EFI_FORWARD_DECLARATION (EFI_DEBUGPORT_PROTOCOL);

//
// DebugPort member functions
//
typedef
EFI_STATUS
(EFIAPI *EFI_DEBUGPORT_RESET) (
  IN EFI_DEBUGPORT_PROTOCOL               *This
  );

typedef
EFI_STATUS
(EFIAPI *EFI_DEBUGPORT_WRITE) (
  IN EFI_DEBUGPORT_PROTOCOL               *This,
  IN UINT32                               Timeout,
  IN OUT UINTN                            *BufferSize,
  IN VOID                                 *Buffer
  );

typedef
EFI_STATUS
(EFIAPI *EFI_DEBUGPORT_READ) (
  IN EFI_DEBUGPORT_PROTOCOL               *This,
  IN UINT32                               Timeout,
  IN OUT UINTN                            *BufferSize,
  OUT VOID                                *Buffer
  );

typedef
EFI_STATUS
(EFIAPI *EFI_DEBUGPORT_POLL) (
  IN EFI_DEBUGPORT_PROTOCOL               *This
  );
  
//
// DebugPort protocol definition
//
typedef struct _EFI_DEBUGPORT_PROTOCOL {
  EFI_DEBUGPORT_RESET                     Reset;
  EFI_DEBUGPORT_WRITE                     Write;
  EFI_DEBUGPORT_READ                      Read;
  EFI_DEBUGPORT_POLL                      Poll;
} EFI_DEBUGPORT_PROTOCOL;

//
// DEBUGPORT variable definitions...
//
#define EFI_DEBUGPORT_VARIABLE_NAME L"DEBUGPORT"
#define EFI_DEBUGPORT_VARIABLE_GUID EFI_DEBUGPORT_PROTOCOL_GUID
#define gEfiDebugPortVariableGuid DebugPortProtocol

//
// DebugPort device path definitions...
//

#define DEVICE_PATH_MESSAGING_DEBUGPORT EFI_DEBUGPORT_PROTOCOL_GUID
#define gEfiDebugPortDevicePathGuid DebugPortProtocol

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL                Header;
  EFI_GUID                                Guid;
} DEBUGPORT_DEVICE_PATH;

#endif /* _DEBUG_PORT_H_ */

