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

  BusSpecificDriverOverride.h

Abstract:

  Bus Specific Driver Override protocol as defined in the EFI 1.1 specification.

--*/

#ifndef _EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL_H_
#define _EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL_H_

//
// Global ID for the Bus Specific Driver Override Protocol
//
#define EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL_GUID \
  { 0x3bc1b285, 0x8a15, 0x4a82, 0xaa, 0xbf, 0x4d, 0x7d, 0x13, 0xfb, 0x32, 0x65 }

EFI_INTERFACE_DECL(_EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL);

//
// Prototypes for the Bus Specific Driver Override Protocol
//

typedef
EFI_STATUS
(EFIAPI *EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_GET_DRIVER) (
  IN     struct _EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL  *This,
  IN OUT EFI_HANDLE                                         *DriverImageHandle
  );

//
// Interface structure for the Bus Specific Driver Override Protocol
//

typedef struct _EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL {
  EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_GET_DRIVER  GetDriver;
} EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL;

extern EFI_GUID BusSpecificDriverOverrideProtocol;

#endif
