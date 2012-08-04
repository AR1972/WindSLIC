#ifndef _EFI_GPT_H
#define _EFI_GPT_H
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

    EfiGpt.h
    
Abstract:
    Include file for EFI partitioning scheme



Revision History

--*/

#define PRIMARY_PART_HEADER_LBA         1

typedef struct {
    EFI_TABLE_HEADER    Header;
    EFI_LBA             MyLBA;
    EFI_LBA             AlternateLBA;
    EFI_LBA             FirstUsableLBA;
    EFI_LBA             LastUsableLBA;
    EFI_GUID            DiskGUID;
    EFI_LBA             PartitionEntryLBA;
    UINT32              NumberOfPartitionEntries;
    UINT32              SizeOfPartitionEntry;
    UINT32              PartitionEntryArrayCRC32;
} EFI_PARTITION_TABLE_HEADER;

#define EFI_PTAB_HEADER_ID  "EFI PART"

typedef struct {
    EFI_GUID    PartitionTypeGUID;
    EFI_GUID    UniquePartitionGUID;
    EFI_LBA     StartingLBA;
    EFI_LBA     EndingLBA;
    UINT64      Attributes;
    CHAR16      PartitionName[36];
} EFI_PARTITION_ENTRY;

//
// EFI Partition Attributes
//
#define EFI_PART_REQUIRED_TO_FUNCTION   0x0000000000000001

#define EFI_PART_TYPE_UNUSED_GUID   \
    { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
    
#define EFI_PART_TYPE_EFI_SYSTEM_PART_GUID  \
    { 0xc12a7328, 0xf81f, 0x11d2, 0xba, 0x4b, 0x00, 0xa0, 0xc9, 0x3e, 0xc9, 0x3b }

#define EFI_PART_TYPE_LEGACY_MBR_GUID   \
    { 0x024dee41, 0x33e7, 0x11d3, 0x9d, 0x69, 0x00, 0x08, 0xc7, 0x81, 0xf3, 0x9f }

#endif