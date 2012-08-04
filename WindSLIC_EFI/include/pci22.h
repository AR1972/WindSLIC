#ifndef _PCI22_H
#define _PCI22_H

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

    pci22.h
    
Abstract:      
    Support for PCI 2.2 standard.




Revision History

--*/
#define PCI_MAX_SEGMENT 0

#ifdef SOFT_SDV
#define PCI_MAX_BUS     1
#else
#define PCI_MAX_BUS     255
#endif

#define PCI_MAX_DEVICE  31
#define PCI_MAX_FUNC    7

//
// Command
//
#define PCI_VGA_PALETTE_SNOOP_DISABLED   0x20

#pragma pack(1)
typedef struct {
    UINT16      VendorId;
    UINT16      DeviceId;
    UINT16      Command;
    UINT16      Status;
    UINT8       RevisionID;
    UINT8       ClassCode[3];
    UINT8       CacheLineSize;
    UINT8       LatencyTimer;
    UINT8       HeaderType;
    UINT8       BIST;
} PCI_DEVICE_INDEPENDENT_REGION;

typedef struct {
    UINT32      Bar[6];
    UINT32      CISPtr;
    UINT16      SubsystemVendorID;
    UINT16      SubsystemID;
    UINT32      ExpansionRomBar;
    UINT32      Reserved[2];
    UINT8       InterruptLine;
    UINT8       InterruptPin;
    UINT8       MinGnt;
    UINT8       MaxLat;     
} PCI_DEVICE_HEADER_TYPE_REGION;

typedef struct {
    PCI_DEVICE_INDEPENDENT_REGION   Hdr;
    PCI_DEVICE_HEADER_TYPE_REGION   Device;
} PCI_TYPE00;

typedef struct {              
    UINT32      Bar[2];
    UINT8       PrimaryBus;
    UINT8       SecondaryBus;
    UINT8       SubordinateBus;
    UINT8       SecondaryLatencyTimer;
    UINT8       IoBase;
    UINT8       IoLimit;
    UINT16      SecondaryStatus;
    UINT16      MemoryBase;
    UINT16      MemoryLimit;
    UINT16      PrefetchableMemoryBase;
    UINT16      PrefetchableMemoryLimit;
    UINT32      PrefetchableBaseUpper32;
    UINT32      PrefetchableLimitUpper32;
    UINT16      IoBaseUpper16;
    UINT16      IoLimitUpper16;
    UINT32      Reserved;
    UINT32      ExpansionRomBAR;
    UINT8       InterruptLine;
    UINT8       InterruptPin;
    UINT16      BridgeControl;
} PCI_BRIDGE_CONTROL_REGISTER;

typedef struct {
  UINT32        CardBusSocketReg;          // Cardus Socket/ExCA Base 
                                           //Address Register
  UINT16        Reserved;  
  UINT16        SecondaryStatus;           // Secondary Status
  UINT8         PciBusNumber;              // PCI Bus Number
  UINT8         CardBusBusNumber;          // CardBus Bus Number
  UINT8         SubordinateBusNumber;      // Subordinate Bus Number
  UINT8         CardBusLatencyTimer;       // CardBus Latency Timer
  UINT32        MemoryBase0;               // Memory Base Register 0
  UINT32        MemoryLimit0;              // Memory Limit Register 0
  UINT32        MemoryBase1;              
  UINT32        MemoryLimit1;              
  UINT32        IoBase0;                   
  UINT32        IoLimit0;                  // I/O Base Register 0
  UINT32        IoBase1;                   // I/O Limit Register 0
  UINT32        IoLimit1;
  UINT8         InterruptLine;             // Interrupt Line
  UINT8         InterruptPin;              // Interrupt Pin
  UINT16        BridgeControl;             // Bridge Control
} PCI_CARDBUS_CONTROL_REGISTER;


#define PCI_CLASS_DISPLAY_CTRL          0x03
#define PCI_CLASS_VGA                   0x00
#define PCI_CLASS_GFX                   0x80
#define PCI_CLASS_MASS_STORAGE		0x01
#define PCI_CLASS_IDE			0x01


#define PCI_CLASS_BRIDGE                0x06
#define PCI_CLASS_ISA                   0x01
#define PCI_CLASS_ISA_POSITIVE_DECODE   0x80

#define PCI_CLASS_NETWORK               0x02 
#define PCI_CLASS_ETHERNET              0x00
        
#define HEADER_TYPE_DEVICE              0x00
#define HEADER_TYPE_PCI_TO_PCI_BRIDGE   0x01
#define HEADER_TYPE_CARDBUS_BRIDGE      0x02

#define HEADER_TYPE_MULTI_FUNCTION      0x80
#define HEADER_LAYOUT_CODE              0x7f

#define IS_PCI_BRIDGE(_p) ((((_p)->Hdr.HeaderType) & HEADER_LAYOUT_CODE) == HEADER_TYPE_PCI_TO_PCI_BRIDGE)        
#define IS_CARDBUS_BRIDGE(_p) ((((_p)->Hdr.HeaderType) & HEADER_LAYOUT_CODE) == HEADER_TYPE_CARDBUS_BRIDGE)
#define IS_PCI_MULTI_FUNC(_p)   (((_p)->Hdr.HeaderType) & HEADER_TYPE_MULTI_FUNCTION)         
#define IS_PCI_VGA(_p)   (((_p)->Hdr.ClassCode)[0] == 0 && ((_p)->Hdr.ClassCode)[1] == PCI_CLASS_VGA && ((_p)->Hdr.ClassCode)[2] == PCI_CLASS_DISPLAY_CTRL )
#define IS_PCI_GFX(_p)   ((_p)->Hdr.ClassCode[0] == 0 && (_p)->Hdr.ClassCode[1] == PCI_CLASS_GFX && (_p)->Hdr.ClassCode[2] == PCI_CLASS_DISPLAY_CTRL )
#define IS_PCI_LPC(_p)   ((_p)->Hdr.ClassCode[0] == 0 && (_p)->Hdr.ClassCode[1] == PCI_CLASS_ISA && (_p)->Hdr.ClassCode[2] == PCI_CLASS_BRIDGE )
#define IS_PCI_IDE(_p)   ((_p)->Hdr.ClassCode[1] == PCI_CLASS_IDE && (_p)->Hdr.ClassCode[2] == PCI_CLASS_MASS_STORAGE )

             
#define PCI_DEVICE_ROMBAR     0x30
#define PCI_BRIDGE_ROMBAR     0x38             

#define PCI_MAX_BAR            6      
#define PCI_MAX_CONFIG_OFFSET  0x100  

#define PCI_COMMAND_REGISTER_OFFSET        4
#define PCI_BRIDGE_CONTROL_REGISTER_OFFSET 0x3E
       

typedef struct {
    PCI_DEVICE_INDEPENDENT_REGION   Hdr;
    PCI_BRIDGE_CONTROL_REGISTER     Bridge;
} PCI_TYPE01;

typedef struct {
    PCI_DEVICE_INDEPENDENT_REGION   Hdr;
    PCI_CARDBUS_CONTROL_REGISTER    CardBusBridge;
} PCI_TYPE02;

typedef struct {
    UINT8   Register;
    UINT8   Function;
    UINT8   Device;
    UINT8   Bus;
    UINT8   Reserved[4];
} DEFIO_PCI_ADDR;

typedef struct {
    UINT32  Reg     : 8;
    UINT32  Func    : 3;
    UINT32  Dev     : 5;
    UINT32  Bus     : 8;
    UINT32  Reserved: 7;
    UINT32  Enable  : 1;
} PCI_CONFIG_ACCESS_CF8;

#pragma pack()

#define EFI_ROOT_BRIDGE_LIST    'eprb'
#define PCI_EXPANSION_ROM_HEADER_SIGNATURE        0xaa55
#define EFI_PCI_EXPANSION_ROM_HEADER_EFISIGNATURE 0x0EF1
#define PCI_DATA_STRUCTURE_SIGNATURE              EFI_SIGNATURE_32('P','C','I','R')
#define PCI_CODE_TYPE_EFI_IMAGE                   0x03
#define EFI_PCI_EXPANSION_ROM_HEADER_COMPRESSED   0x0001

#define EFI_PCI_COMMAND_IO_SPACE                     0x0001
#define EFI_PCI_COMMAND_MEMORY_SPACE                 0x0002
#define EFI_PCI_COMMAND_BUS_MASTER                   0x0004
#define EFI_PCI_COMMAND_SPECIAL_CYCLE                0x0008
#define EFI_PCI_COMMAND_MEMORY_WRITE_AND_INVALIDATE  0x0010
#define EFI_PCI_COMMAND_VGA_PALETTE_SNOOP            0x0020
#define EFI_PCI_COMMAND_PARITY_ERROR_RESPOND         0x0040
#define EFI_PCI_COMMAND_STEPPING_CONTROL             0x0080
#define EFI_PCI_COMMAND_SERR                         0x0100
#define EFI_PCI_COMMAND_FAST_BACK_TO_BACK            0x0200



#define EFI_PCI_BRIDGE_CONTROL_PARITY_ERROR_RESPONSE         0x0001
#define EFI_PCI_BRIDGE_CONTROL_SERR                          0x0002
#define EFI_PCI_BRIDGE_CONTROL_ISA                           0x0004
#define EFI_PCI_BRIDGE_CONTROL_VGA                           0x0008
#define EFI_PCI_BRIDGE_CONTROL_MASTER_ABORT                  0x0010
#define EFI_PCI_BRIDGE_CONTROL_RESET_SECONDARY_BUS           0x0020
#define EFI_PCI_BRIDGE_CONTROL_FAST_BACK_TO_BACK             0x0040
#define EFI_PCI_BRIDGE_CONTROL_PRIMARY_DISCARD_TIMER         0x0080
#define EFI_PCI_BRIDGE_CONTROL_SECONDARY_DISCARD_TIMER       0x0100
#define EFI_PCI_BRIDGE_CONTROL_TIMER_STATUS                  0x0200
#define EFI_PCI_BRIDGE_CONTROL_DISCARD_TIMER_SERR            0x0400

//
// Following are the PCI-CARDBUS bridge control bit
//
#define EFI_PCI_BRIDGE_CONTROL_IREQINT_ENABLE                0x0040 
#define EFI_PCI_BRIDGE_CONTROL_RANGE0_MEMORY_TYPE            0x0080
#define EFI_PCI_BRIDGE_CONTROL_RANGE1_MEMORY_TYPE            0x0100
#define EFI_PCI_BRIDGE_CONTROL_WRITE_POSTING_ENABLE          0x0200

//
// Following are the PCI status control bit 
//
#define EFI_PCI_STATUS_CAPABILITY                            0x0010
#define EFI_PCI_STATUS_66MZ_CAPABLE                          0x0020
#define EFI_PCI_FAST_BACK_TO_BACK_CAPABLE                    0x0040
#define EFI_PCI_MASTER_DATA_PARITY_ERROR                     0x0080

#define EFI_PCI_CAPABILITY_PTR                               0x34
#define EFI_PCI_CARDBUS_BRIDGE_CAPABILITY_PTR                0x14

#define EFI_PCI_POWER_MANAGEMENT_CAPID                       0x01

#pragma pack(1)
typedef struct {
    UINT16          Signature;              // 0xaa55
    UINT8           Reserved[0x16];
    UINT16          PcirOffset;
} PCI_EXPANSION_ROM_HEADER;


typedef struct {
    UINT16          Signature;              // 0xaa55
    UINT16          InitializationSize;
    UINT32          EfiSignature;           // 0x0EF1
    UINT16          EfiSubsystem;
    UINT16          EfiMachineType;
    UINT16          CompressionType;
    UINT8           Reserved[8];
    UINT16          EfiImageHeaderOffset;
    UINT16          PcirOffset;
} EFI_PCI_EXPANSION_ROM_HEADER;

typedef struct {
    UINT32          Signature;              // "PCIR" 
    UINT16          VendorId;
    UINT16          DeviceId;
    UINT16          Reserved0;
    UINT16          Length;
    UINT8           Revision;
    UINT8           ClassCode[3];
    UINT16          ImageLength;
    UINT16          CodeRevision;
    UINT8           CodeType;
    UINT8           Indicator;
    UINT16          Reserved1;
} PCI_DATA_STRUCTURE;

typedef struct {
    UINT8         CapabilityID;
    UINT8         NextItemPtr;
    UINT16        PMC;
    UINT16        PMCSR;
    UINT8         BridgeExtention;
    UINT8         Data;
} EFI_PCI_POWER_MANAGEMENT_REGISTER_BLOCK;

#pragma pack()

#endif
    




    
