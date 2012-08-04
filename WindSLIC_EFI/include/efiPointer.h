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

  SimplePointer.h

Abstract:

  Simple Pointer protocol from the EFI 1.1 specification.

  Abstraction of a very simple pointer device like a mice or tracekballs.

--*/

#ifndef _EFI_POINTER_H_
#define _EFI_POINTER_H_

#define EFI_SIMPLE_POINTER_PROTOCOL_GUID \
    {0x31878c87,0xb75,0x11d5,0x9a,0x4f,0x0,0x90,0x27,0x3f,0xc1,0x4d}

EFI_INTERFACE_DECL(_EFI_SIMPLE_POINTER_PROTOCOL);

//
// Data structures
//

typedef struct {
  INT32       RelativeMovementX;
  INT32       RelativeMovementY;
  INT32       RelativeMovementZ;
  BOOLEAN     LeftButton;
  BOOLEAN     RightButton;
} EFI_SIMPLE_POINTER_STATE;

typedef struct {
  UINT64      ResolutionX;
  UINT64      ResolutionY;
  UINT64      ResolutionZ;
  BOOLEAN     LeftButton;
  BOOLEAN     RightButton;
} EFI_SIMPLE_POINTER_MODE;

typedef
EFI_STATUS
(EFIAPI *EFI_SIMPLE_POINTER_RESET) (
  IN struct _EFI_SIMPLE_POINTER_PROTOCOL    *This,
  IN BOOLEAN                                ExtendedVerification
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SIMPLE_POINTER_GET_STATE) (
  IN struct _EFI_SIMPLE_POINTER_PROTOCOL  *This,
  IN OUT EFI_SIMPLE_POINTER_STATE         *State
  );

typedef struct _EFI_SIMPLE_POINTER_PROTOCOL {
  EFI_SIMPLE_POINTER_RESET      Reset;
  EFI_SIMPLE_POINTER_GET_STATE  GetState;
  EFI_EVENT                     WaitForInput;
  EFI_SIMPLE_POINTER_MODE       *Mode;
} EFI_SIMPLE_POINTER_PROTOCOL;

extern EFI_GUID SimplePointerProtocol;

#endif
