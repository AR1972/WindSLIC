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

  PlatformDriverOverride.h

Abstract:

  Platform Driver Override protocol as defined in the EFI 1.1 specification.

--*/

#ifndef _EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL_H_
#define _EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL_H_

//
// Global ID for the Platform Driver Override Protocol
//
#define EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL_GUID \
    { 0x6b30c738, 0xa391, 0x11d4, 0x9a, 0x3b, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d}

EFI_INTERFACE_DECL(_EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL);

//
// Prototypes for the Platform Driver Override Protocol
//

typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_DRIVER_OVERRIDE_GET_DRIVER) (
  IN     struct _EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL  *This,
  IN     EFI_HANDLE                                     ControllerHandle,
  IN OUT EFI_HANDLE                                     *DriverImageHandle
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_DRIVER_OVERRIDE_GET_DRIVER_PATH) (
  IN     struct _EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL  *This,
  IN     EFI_HANDLE                                     ControllerHandle,
  IN OUT EFI_DEVICE_PATH_PROTOCOL                       **DriverImagePath
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_DRIVER_OVERRIDE_DRIVER_LOADED) (
  IN struct _EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL  *This,
  IN EFI_HANDLE                                     ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL                       *DriverImagePath,
  IN EFI_HANDLE                                     DriverImageHandle
  );

//
// Interface structure for the Platform Driver Override Protocol
//
typedef struct _EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL {
  EFI_PLATFORM_DRIVER_OVERRIDE_GET_DRIVER       GetDriver;
  EFI_PLATFORM_DRIVER_OVERRIDE_GET_DRIVER_PATH  GetDriverPath;
  EFI_PLATFORM_DRIVER_OVERRIDE_DRIVER_LOADED    DriverLoaded;
} EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL;

extern EFI_GUID EFI_PS2_POLICY_PROTOCOL;

#endif
