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

    UsbIo.h

Abstract:

    EFI Usb I/O Protocol

Revision History

--*/

#ifndef _EFI_USB_IO_H
#define _EFI_USB_IO_H

#include "usb.h"

//
// Global ID for the USB I/O Protocol
//
#define EFI_USB_IO_PROTOCOL_GUID \
    { 0x2B2F68D6, 0x0CD2, 0x44cf, 0x8E, 0x8B, 0xBB, 0xA2, 0x0B, 0x1B, 0x5B, 0x75 }

EFI_INTERFACE_DECL(_EFI_USB_IO_PROTOCOL);

//
//  Asynchronous USB transfer call back function
//
typedef
EFI_STATUS
(EFIAPI *EFI_ASYNC_USB_TRANSFER_CALLBACK) (
  IN VOID         *Data,
  IN UINTN        DataLength,
  IN VOID         *Context,
  IN UINT32       Status
);

//
// Prototype for EFI USB I/O protocol
//
typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_CONTROL_TRANSFER) (
  IN struct _EFI_USB_IO_PROTOCOL    *This,
  IN EFI_USB_DEVICE_REQUEST         *Request,
  IN EFI_USB_DATA_DIRECTION         Direction,
  IN UINT32                         Timeout,
  IN OUT VOID                       *Data       OPTIONAL,
  IN UINTN                          DataLength  OPTIONAL,
  OUT UINT32                        *Status
  );

typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_BULK_TRANSFER) (
  IN struct _EFI_USB_IO_PROTOCOL    *This,
  IN UINT8                          DeviceEndpoint,
  IN OUT VOID                       *Data,
  IN OUT UINTN                      *DataLength,
  IN UINTN                          Timeout,
  OUT UINT32                        *Status
);

typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_ASYNC_INTERRUPT_TRANSFER) (
  IN struct _EFI_USB_IO_PROTOCOL      *This,
  IN UINT8                            DeviceEndpoint,
  IN BOOLEAN                          IsNewTransfer,
  IN UINTN                            PollingInterval    OPTIONAL,
  IN UINTN                            DataLength         OPTIONAL,
  IN EFI_ASYNC_USB_TRANSFER_CALLBACK  InterruptCallBack  OPTIONAL,
  IN VOID                             *Context           OPTIONAL
);

typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_SYNC_INTERRUPT_TRANSFER) (
  IN struct _EFI_USB_IO_PROTOCOL    *This,
  IN     UINT8                      DeviceEndpoint,
  IN OUT VOID                       *Data,
  IN OUT UINTN                      *DataLength,
  IN     UINTN                      Timeout,
  OUT    UINT32                     *Status
);

typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_ISOCHRONOUS_TRANSFER) (
  IN struct _EFI_USB_IO_PROTOCOL    *This,
  IN     UINT8                      DeviceEndpoint,
  IN OUT VOID                       *Data,
  IN     UINTN                      DataLength,
  OUT    UINT32                     *Status
);

typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_ASYNC_ISOCHRONOUS_TRANSFER) (
  IN struct _EFI_USB_IO_PROTOCOL      *This,
  IN UINT8                            DeviceEndpoint,
  IN OUT VOID                         *Data,
  IN     UINTN                        DataLength,
  IN EFI_ASYNC_USB_TRANSFER_CALLBACK  IsochronousCallBack,
  IN VOID                             *Context   OPTIONAL
);

typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_PORT_RESET) (
  IN struct _EFI_USB_IO_PROTOCOL    *This
);

typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_GET_DEVICE_DESCRIPTOR) (
  IN  struct _EFI_USB_IO_PROTOCOL   *This,
  OUT EFI_USB_DEVICE_DESCRIPTOR     *DeviceDescriptor
);

typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_GET_CONFIG_DESCRIPTOR) (
  IN  struct _EFI_USB_IO_PROTOCOL   *This,
  OUT EFI_USB_CONFIG_DESCRIPTOR     *ConfigurationDescriptor
);

typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_GET_INTERFACE_DESCRIPTOR) (
  IN  struct _EFI_USB_IO_PROTOCOL   *This,
  OUT EFI_USB_INTERFACE_DESCRIPTOR  *InterfaceDescriptor
);

typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_GET_ENDPOINT_DESCRIPTOR) (
  IN  struct _EFI_USB_IO_PROTOCOL   *This,
  IN  UINT8                         EndpointIndex,
  OUT EFI_USB_ENDPOINT_DESCRIPTOR   *EndpointDescriptor
);

typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_GET_STRING_DESCRIPTOR) (
  IN  struct _EFI_USB_IO_PROTOCOL   *This,
  IN  UINT16                        LangID,
  IN  UINT8                         StringID,
  OUT CHAR16                        **String
);

typedef
EFI_STATUS
(EFIAPI *EFI_USB_IO_GET_SUPPORTED_LANGUAGE) (
  IN  struct _EFI_USB_IO_PROTOCOL   *This,
  OUT UINT16                        **LangIDTable,
  OUT UINT16                        *TableSize
);

//
//  Protocol Interface Structure
//
typedef struct _EFI_USB_IO_PROTOCOL {
  //
  // IO transfer
  //
  EFI_USB_IO_CONTROL_TRANSFER             UsbControlTransfer;
  EFI_USB_IO_BULK_TRANSFER                UsbBulkTransfer;
  EFI_USB_IO_ASYNC_INTERRUPT_TRANSFER     UsbAsyncInterruptTransfer;
  EFI_USB_IO_SYNC_INTERRUPT_TRANSFER      UsbSyncInterruptTransfer;
  EFI_USB_IO_ISOCHRONOUS_TRANSFER         UsbIsochronousTransfer;
  EFI_USB_IO_ASYNC_ISOCHRONOUS_TRANSFER   UsbAsyncIsochronousTransfer;
  
  //
  // Common device request
  //
  EFI_USB_IO_GET_DEVICE_DESCRIPTOR        UsbGetDeviceDescriptor;
  EFI_USB_IO_GET_CONFIG_DESCRIPTOR        UsbGetConfigDescriptor;
  EFI_USB_IO_GET_INTERFACE_DESCRIPTOR     UsbGetInterfaceDescriptor;
  EFI_USB_IO_GET_ENDPOINT_DESCRIPTOR      UsbGetEndpointDescriptor;
  EFI_USB_IO_GET_STRING_DESCRIPTOR        UsbGetStringDescriptor;
  EFI_USB_IO_GET_SUPPORTED_LANGUAGE       UsbGetSupportedLanguages;

  //
  // Reset controller's parent port
  //
  EFI_USB_IO_PORT_RESET                   UsbPortReset;
} EFI_USB_IO_PROTOCOL;

extern EFI_GUID UsbIoProtocol;

#endif
