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

    DriverDiagnostics.h
    
Abstract:

    EFI Driver Diagnostics Protocol

Revision History

--*/

#ifndef _EFI_DRIVER_DIAGNOSTICS_H_
#define _EFI_DRIVER_DIAGNOSTICS_H_

//
// Global ID for the Driver Diagnostics Protocol
//
#define EFI_DRIVER_DIAGNOSTICS_PROTOCOL_GUID    \
  { 0x0784924f, 0xe296, 0x11d4, 0x9a, 0x49, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d }
  
EFI_INTERFACE_DECL(_EFI_DRIVER_DIAGNOSTICS_PROTOCOL);

typedef enum {
  EfiDriverDiagnosticTypeStandard               = 0,
  EfiDriverDiagnosticTypeExtended               = 1,
  EfiDriverDiagnosticTypeManufacturing          = 2,
  EfiDriverDiagnosticTypeMaximum
} EFI_DRIVER_DIAGNOSTIC_TYPE;

typedef
EFI_STATUS
(EFIAPI *EFI_DRIVER_DIAGNOSTICS_RUN_DIAGNOSTICS) (
  IN  struct _EFI_DRIVER_DIAGNOSTICS_PROTOCOL  *This,
  IN  EFI_HANDLE                               ControllerHandle,
  IN  EFI_HANDLE                               ChildHandle  OPTIONAL,
  IN  EFI_DRIVER_DIAGNOSTIC_TYPE               DiagnosticType,
  IN  CHAR8                                    *Language,
  OUT EFI_GUID                                 **ErrorType,
  OUT UINTN                                    *BufferSize, 
  OUT CHAR16                                   **Buffer
  );
/*++

  Routine Description:
    Runs diagnostics on a controller.

  Arguments:
    This             - A pointer to the EFI_DRIVER_DIAGNOSTICS_PROTOCOL instance.
    ControllerHandle - The handle of the controller to run diagnostics on.
    ChildHandle      - The handle of the child controller to run diagnostics on  
                       This is an optional parameter that may be NULL.  It will 
                       be NULL for device drivers.  It will also be NULL for a 
                       bus drivers that wish to run diagnostics on the bus 
                       controller.  It will not be NULL for a bus driver that 
                       wishes to run diagnostics on one of its child controllers.
    DiagnosticType   - Indicates type of diagnostics to perform on the controller 
                       specified by ControllerHandle and ChildHandle.   See 
                       "Related Definitions" for the list of supported types.
    Language         - A pointer to a three character ISO 639-2 language 
                       identifier.  This is the language in which the optional 
                       error message should be returned in Buffer, and it must 
                       match one of the languages specified in SupportedLanguages.
                       The number of languages supported by a driver is up to 
                       the driver writer.  
    ErrorType        - A GUID that defines the format of the data returned in 
                       Buffer.  
    BufferSize       - The size, in bytes, of the data returned in Buffer.  
    Buffer           - A buffer that contains a Null-terminated Unicode string 
                       plus some additional data whose format is defined by 
                       ErrorType.  Buffer is allocated by this function with 
                       AllocatePool(), and it is the caller's responsibility 
                       to free it with a call to FreePool().  

  Returns:
    EFI_SUCCESS           - The controller specified by ControllerHandle and 
                            ChildHandle passed the diagnostic.
    EFI_INVALID_PARAMETER - ControllerHandle is not a valid EFI_HANDLE.
    EFI_INVALID_PARAMETER - ChildHandle is not NULL and it is not a valid 
                            EFI_HANDLE.
    EFI_INVALID_PARAMETER - Language is NULL.
    EFI_INVALID_PARAMETER - ErrorType is NULL.
    EFI_INVALID_PARAMETER - BufferType is NULL.
    EFI_INVALID_PARAMETER - Buffer is NULL.
    EFI_UNSUPPORTED       - The driver specified by This does not support 
                            running diagnostics for the controller specified 
                            by ControllerHandle and ChildHandle.
    EFI_UNSUPPORTED       - The driver specified by This does not support the 
                            type of diagnostic specified by DiagnosticType.
    EFI_UNSUPPORTED       - The driver specified by This does not support the 
                            language specified by Language.
    EFI_OUT_OF_RESOURCES  - There are not enough resources available to complete
                            the diagnostics.
    EFI_OUT_OF_RESOURCES  - There are not enough resources available to return
                            the status information in ErrorType, BufferSize, 
                            and Buffer.
    EFI_DEVICE_ERROR      - The controller specified by ControllerHandle and 
                            ChildHandle did not pass the diagnostic.

--*/

//
// Interface structure for the Driver Diagnostics Protocol
//

typedef struct _EFI_DRIVER_DIAGNOSTICS_PROTOCOL {
  EFI_DRIVER_DIAGNOSTICS_RUN_DIAGNOSTICS  RunDiagnostics;
  CHAR8                                   *SupportedLanguages;
} EFI_DRIVER_DIAGNOSTICS_PROTOCOL;

/*++

  Protocol Description:
    Used to perform diagnostics on a controller that an EFI Driver is managing.

  Parameters:
    RunDiagnostics     - Runs diagnostics on a controller.
    SupportedLanguages - A Null-terminated ASCII string that contains one or more
                         ISO 639-2 language codes.  This is the list of language 
                         codes that this protocol supports.

--*/

extern EFI_GUID DriverDiagnosticsProtocol;

#endif
