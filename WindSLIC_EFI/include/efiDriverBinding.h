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

    DriverBinding.h
    
Abstract:

    EFI ControllerHandle Driver Protocol

Revision History

--*/

#ifndef _EFI_DRIVER_BINDING_H_
#define _EFI_DRIVER_BINDING_H_

//
// Global ID for the ControllerHandle Driver Protocol
//
#define EFI_DRIVER_BINDING_PROTOCOL_GUID    \
  { 0x18a031ab, 0xb443, 0x4d1a, 0xa5, 0xc0, 0xc, 0x9, 0x26, 0x1e, 0x9f, 0x71 }

EFI_INTERFACE_DECL(_EFI_DRIVER_BINDING_PROTOCOL);

typedef
EFI_STATUS
(EFIAPI *EFI_DRIVER_BINDING_SUPPORTED) (
  IN struct _EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                             ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL               *RemainingDevicePath  OPTIONAL
  )
/*++

  Routine Description:
    Test to see if this driver supports ControllerHandle. 

  Arguments:
    This                - Protocol instance pointer.
    ControllerHandle    - Handle of device to test
    RemainingDevicePath - Optional parameter use to pick a specific child 
                          device to start.

  Returns:
    EFI_SUCCES          - This driver supports this device
    EFI_ALREADY_STARTED - This driver is already running on this device
    other               - This driver does not support this device

--*/
;

typedef
EFI_STATUS
(EFIAPI *EFI_DRIVER_BINDING_START) (
  IN struct _EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                             ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL               *RemainingDevicePath  OPTIONAL
  )
/*++

  Routine Description:
    Start this driver on ControllerHandle.

  Arguments:
    This                - Protocol instance pointer.
    ControllerHandle    - Handle of device to bind driver to
    RemainingDevicePath - Optional parameter use to pick a specific child 
                          device to start.

  Returns:
    EFI_SUCCES          - This driver is added to ControllerHandle
    EFI_ALREADY_STARTED - This driver is already running on ControllerHandle
    other               - This driver does not support this device

--*/
;

typedef
EFI_STATUS
(EFIAPI *EFI_DRIVER_BINDING_STOP) (
  IN  struct _EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN  EFI_HANDLE                            ControllerHandle,
  IN  UINTN                                 NumberOfChildren,
  IN  EFI_HANDLE                            *ChildHandleBuffer
  )
/*++

  Routine Description:
    Stop this driver on ControllerHandle.

  Arguments:
    This              - Protocol instance pointer.
    ControllerHandle  - Handle of device to stop driver on 
    NumberOfChildren  - Number of Handles in ChildHandleBuffer. If number of 
                        children is zero stop the entire bus driver.
    ChildHandleBuffer - List of Child Handles to Stop.

  Returns:
    EFI_SUCCES          - This driver is removed ControllerHandle
    other               - This driver was not removed from this device

--*/
;


//
// Interface structure for the ControllerHandle Driver Protocol
//

typedef struct _EFI_DRIVER_BINDING_PROTOCOL {
  EFI_DRIVER_BINDING_SUPPORTED    Supported;
  EFI_DRIVER_BINDING_START        Start;
  EFI_DRIVER_BINDING_STOP         Stop;
  UINT32                          Version;
  EFI_HANDLE                      ImageHandle;
  EFI_HANDLE                      DriverBindingHandle;
} EFI_DRIVER_BINDING_PROTOCOL;

extern EFI_GUID DriverBindingProtocol;

#endif
