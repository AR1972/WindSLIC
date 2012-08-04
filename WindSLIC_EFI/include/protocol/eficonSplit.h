#ifndef _EFI_CONFORK_H
#define _EFI_CONFORK_H
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

Abstract:



Revision History

--*/



//
// ConOut Forker Protocol
//

#define TEXT_OUT_SPLITER_PROTOCOL    \
    { 0x56d830a0, 0x7e7a, 0x11d3, 0xbb, 0xa0, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b }

#define ERROR_OUT_SPLITER_PROTOCOL    \
    { 0xf0ba9039, 0x68f1, 0x425e, 0xaa, 0x7f, 0xd9, 0xaa, 0xf9, 0x1b, 0x82, 0xa1}

#define TEXT_IN_SPLITER_PROTOCOL    \
    { 0xf9a3c550, 0x7fb5, 0x11d3, 0xbb, 0xa0, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b }

#endif
