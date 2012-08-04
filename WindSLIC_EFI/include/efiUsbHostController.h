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

  UsbHostController.h

Abstract:

   
--*/

#ifndef _USB_HOSTCONTROLLER_H_
#define _USB_HOSTCONTROLLER_H_

#include "usb.h"
#include "efiUsbIo.h"

#define EFI_USB_HC_PROTOCOL_GUID \
  {0xf5089266, 0x1aa0, 0x4953, 0x97, 0xd8, 0x56, 0x2f, 0x8a, 0x73, 0xb5, 0x19}

//
// Forward reference for pure ANSI compatability
//
EFI_INTERFACE_DECL(_EFI_USB_HC_PROTOCOL);

typedef enum {
  EfiUsbHcStateHalt,
  EfiUsbHcStateOperational,
  EfiUsbHcStateSuspend,
  EfiUsbHcStateMaximum
} EFI_USB_HC_STATE;

#define EFI_USB_HC_RESET_GLOBAL             0x0001
#define EFI_USB_HC_RESET_HOST_CONTROLLER    0x0002

//
// Protocol definitions
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_HC_PROTOCOL_RESET) (
  IN struct _EFI_USB_HC_PROTOCOL    *This,
  IN UINT16                         Attributes
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB_HC_PROTOCOL_GET_STATE) (
  IN struct _EFI_USB_HC_PROTOCOL    *This,
  OUT EFI_USB_HC_STATE              *State
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB_HC_PROTOCOL_SET_STATE) (
  IN struct _EFI_USB_HC_PROTOCOL    *This,
  IN EFI_USB_HC_STATE               State
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB_HC_PROTOCOL_CONTROL_TRANSFER) (
  IN struct _EFI_USB_HC_PROTOCOL    *This,
  IN     UINT8                      DeviceAddress,
  IN     BOOLEAN                    IsSlowDevice,
  IN     UINT8                      MaximumPacketLength,
  IN     EFI_USB_DEVICE_REQUEST     *Request,
  IN     EFI_USB_DATA_DIRECTION     TransferDirection,
  IN OUT VOID                       *Data                 OPTIONAL,
  IN OUT UINTN                      *DataLength           OPTIONAL,  
  IN     UINTN                      TimeOut,
  OUT    UINT32                     *TransferResult
  );
  
typedef
EFI_STATUS
(EFIAPI *EFI_USB_HC_PROTOCOL_BULK_TRANSFER) (
  IN struct _EFI_USB_HC_PROTOCOL    *This,
  IN  UINT8                         DeviceAddress,
  IN  UINT8                         EndPointAddress,
  IN  UINT8                         MaximumPacketLength,
  IN OUT VOID                       *Data,          
  IN OUT UINTN                      *DataLength,    
  IN OUT UINT8                      *DataToggle,    
  IN  UINTN                         TimeOut,        
  OUT UINT32                        *TransferResult
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB_HC_PROTOCOL_ASYNC_INTERRUPT_TRANSFER) (
  IN struct _EFI_USB_HC_PROTOCOL          *This,
  IN     UINT8                            DeviceAddress,
  IN     UINT8                            EndPointAddress,
  IN     BOOLEAN                          IsSlowDevice,
  IN     UINT8                            MaxiumPacketLength,
  IN     BOOLEAN                          IsNewTransfer,
  IN OUT UINT8                            *DataToggle      OPTIONAL,
  IN     UINTN                            PollingInterval  OPTIONAL,
  IN     UINTN                            DataLength       OPTIONAL,
  IN     EFI_ASYNC_USB_TRANSFER_CALLBACK  CallBackFunction OPTIONAL,
  IN     VOID                             *Context         OPTIONAL  
  );
  
typedef
EFI_STATUS
(EFIAPI *EFI_USB_HC_PROTOCOL_SYNC_INTERRUPT_TRANSFER) (
  IN struct _EFI_USB_HC_PROTOCOL    *This,
  IN     UINT8                      DeviceAddress,
  IN     UINT8                      EndPointAddress,
  IN     BOOLEAN                    IsSlowDevice,
  IN     UINT8                      MaximumPacketLength,
  IN OUT VOID                       *Data,
  IN OUT UINTN                      *DataLength,
  IN OUT UINT8                      *DataToggle,
  IN     UINTN                      TimeOut,
  OUT    UINT32                     *TransferResult
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB_HC_PROTOCOL_ISOCHRONOUS_TRANSFER) (
  IN struct _EFI_USB_HC_PROTOCOL    *This,
  IN     UINT8                      DeviceAddress,
  IN     UINT8                      EndPointAddress,
  IN     UINT8                      MaximumPacketLength,
  IN OUT VOID                       *Data,
  IN OUT UINTN                      DataLength,
  OUT    UINT32                     *TransferResult
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB_HC_PROTOCOL_ASYNC_ISOCHRONOUS_TRANSFER) (
  IN struct _EFI_USB_HC_PROTOCOL      *This,
  IN     UINT8                        DeviceAddress,
  IN     UINT8                        EndPointAddress,
  IN     UINT8                        MaximumPacketLength,
  IN OUT VOID                         *Data,
  IN     UINTN                        DataLength,
  IN EFI_ASYNC_USB_TRANSFER_CALLBACK  IsochronousCallBack,
  IN VOID                             *Context   OPTIONAL
  );


typedef 
EFI_STATUS
(EFIAPI *EFI_USB_HC_PROTOCOL_GET_ROOTHUB_PORT_NUMBER) (
  IN struct _EFI_USB_HC_PROTOCOL    *This,
  OUT UINT8                         *PortNumber
  );

typedef 
EFI_STATUS
(EFIAPI *EFI_USB_HC_PROTOCOL_GET_ROOTHUB_PORT_STATUS) (
  IN struct _EFI_USB_HC_PROTOCOL    *This,
  IN  UINT8                         PortNumber,
  OUT EFI_USB_PORT_STATUS           *PortStatus
  );

typedef 
EFI_STATUS
(EFIAPI *EFI_USB_HC_PROTOCOL_SET_ROOTHUB_PORT_FEATURE) (
  IN struct _EFI_USB_HC_PROTOCOL    *This,
  IN UINT8                          PortNumber,
  IN EFI_USB_PORT_FEATURE           PortFeature
  );

typedef 
EFI_STATUS
(EFIAPI *EFI_USB_HC_PROTOCOL_CLEAR_ROOTHUB_PORT_FEATURE) (
  IN struct _EFI_USB_HC_PROTOCOL    *This,
  IN UINT8                          PortNumber,
  IN EFI_USB_PORT_FEATURE           PortFeature
  );

typedef struct _EFI_USB_HC_PROTOCOL {  
  EFI_USB_HC_PROTOCOL_RESET                       Reset;
  EFI_USB_HC_PROTOCOL_GET_STATE                   GetState;
  EFI_USB_HC_PROTOCOL_SET_STATE                   SetState;
  EFI_USB_HC_PROTOCOL_CONTROL_TRANSFER            ControlTransfer;
  EFI_USB_HC_PROTOCOL_BULK_TRANSFER               BulkTransfer;
  EFI_USB_HC_PROTOCOL_ASYNC_INTERRUPT_TRANSFER    AsyncInterruptTransfer;
  EFI_USB_HC_PROTOCOL_SYNC_INTERRUPT_TRANSFER     SyncInterruptTransfer;
  EFI_USB_HC_PROTOCOL_ISOCHRONOUS_TRANSFER        IsochronousTransfer;
  EFI_USB_HC_PROTOCOL_ASYNC_ISOCHRONOUS_TRANSFER  AsyncIsochronousTransfer;
  EFI_USB_HC_PROTOCOL_GET_ROOTHUB_PORT_NUMBER     GetRootHubPortNumber;
  EFI_USB_HC_PROTOCOL_GET_ROOTHUB_PORT_STATUS     GetRootHubPortStatus; 
  EFI_USB_HC_PROTOCOL_SET_ROOTHUB_PORT_FEATURE    SetRootHubPortFeature;
  EFI_USB_HC_PROTOCOL_CLEAR_ROOTHUB_PORT_FEATURE  ClearRootHubPortFeature;
  UINT16                                          MajorRevision;
  UINT16                                          MinorRevision;
  } EFI_USB_HC_PROTOCOL;

extern EFI_GUID UsbHcProtocol;

#endif
