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
  
    Ebc.h
  
Abstract:

  Describes the protocol interface to the EBC interpreter.
  
--*/  
#ifndef _EBC_H_
#define _EBC_H_

#define EFI_EBC_INTERPRETER_PROTOCOL_GUID \
    {0x13AC6DD1, 0x73D0, 0x11D4, 0xB0, 0x6B, 0x00, 0xAA, 0x00, 0xBD, 0x6D, 0xE7}

//
// Define for forward reference.
//
EFI_INTERFACE_DECL(_EFI_EBC_PROTOCOL);

/*++

Routine Description:
  
  Create a thunk for an image entry point. In short, given the physical address
  of the entry point for a loaded image, create a thunk that does some 
  fixup of arguments (and perform any other necessary overhead) and then
  calls the original entry point. The caller can then use the returned pointer
  to the created thunk as the new entry point to image.

Arguments:

  This          - protocol instance pointer
  ImageHandle   - handle to the image. The EBC interpreter may use this to keep
                  track of any resource allocations performed in loading and
                  executing the image.
  EbcEntryPoint - the entry point for the image (as defined in the file header)
  Thunk         - pointer to thunk pointer where the address of the created
                  thunk is returned.

Returns:

  Standard EFI_STATUS

--*/

typedef 
EFI_STATUS
(EFIAPI *EFI_EBC_CREATE_THUNK) (
  IN struct _EFI_EBC_PROTOCOL   *This,
  IN EFI_HANDLE                 ImageHandle,
  IN VOID                       *EbcEntryPoint,
  OUT VOID                      **Thunk
  );

/*++

Routine Description:
  
  Perform any cleanup necessary when an image is unloaded. Basically it gives
  the EBC interpreter the chance to free up any resources allocated during
  load and execution of an EBC image.

Arguments:

  This          - protocol instance pointer
  ImageHandle   - the handle of the image being unloaded.

Returns:

  Standard EFI_STATUS.

--*/

typedef
EFI_STATUS
(EFIAPI *EFI_EBC_UNLOAD_IMAGE) (
  IN struct _EFI_EBC_PROTOCOL   *This,
  IN EFI_HANDLE                 ImageHandle
  );

/*++

Routine Description:
  
  The I-Cache-flush registration service takes a pointer to a function to
  call to flush the I-Cache. Here's the prototype for that function pointer.

Arguments:

  Start         - physical start address of CPU instruction cache to flush.
  Length        - how many bytes to flush of the instruction cache.

Returns:

  Standard EFI_STATUS.

--*/
typedef 
EFI_STATUS
(EFIAPI *EBC_ICACHE_FLUSH) (
  IN EFI_PHYSICAL_ADDRESS     Start,
  IN UINT64                   Length
  );

/*++

Routine Description:
  
  This routine is called by the core firmware to provide the EBC driver with
  a function to call to flush the CPU's instruction cache following creation
  of a thunk. It is not required.

Arguments:

  This      - protocol instance pointer
  Flush     - pointer to the function to call to flush the CPU instruction
              cache.

Returns:

  Standard EFI_STATUS.

--*/
typedef
EFI_STATUS
(EFIAPI *EFI_EBC_REGISTER_ICACHE_FLUSH) (
  IN struct _EFI_EBC_PROTOCOL   *This,
  IN EBC_ICACHE_FLUSH           Flush
  );

/*++

Routine Description:
  
  This routine can be called to get the VM revision. It returns the same
  value as the EBC BREAK 1 instruction returns.

Arguments:

  This      - protocol instance pointer
  Version   - pointer to where to return the VM version

Returns:

  Standard EFI_STATUS.

--*/
typedef
EFI_STATUS
(EFIAPI *EFI_EBC_GET_VERSION) (
  IN struct _EFI_EBC_PROTOCOL   *This,
  IN OUT UINT64                 *Version
  );

//
// Prototype for the actual EBC protocol interface
//
typedef struct _EFI_EBC_PROTOCOL {
  EFI_EBC_CREATE_THUNK              CreateThunk;
  EFI_EBC_UNLOAD_IMAGE              UnloadImage;
  EFI_EBC_REGISTER_ICACHE_FLUSH     RegisterICacheFlush;
  EFI_EBC_GET_VERSION               GetVersion;
} EFI_EBC_PROTOCOL;

//
// Extern the global EBC protocol GUID
//
extern EFI_GUID EbcProtocol;


#endif	
