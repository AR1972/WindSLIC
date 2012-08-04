#ifndef ACPI_H
#define ACPI_H
//
/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */
//
// ACPI signatures
//
#define ACPI_FACP_SIG		0x50434146
#define ACPI_SPIC_SIG		0x43495053
#define ACPI_DSDT_SIG		0x54445344
#define ACPI_FACP_SIG		0x50434146
#define ACPI_FACS_SIG		0x53434146
#define ACPI_PSDT_SIG		0x54445350
#define ACPI_RSDT_SIG		0x54445352
#define ACPI_SSDT_SIG		0x54445353
#define ACPI_SBST_SIG		0x54534253
#define ACPI_RSDP_SIG		0x2052545020445352		// "RSD PTR "
#define ACPI_SLIC_SIG		0x43494c53				// "SLIC"
#define ACPI_XSDT_SIG		0x54445358				// "XSDT"
#define ACPI_BOOT_SIG		0x544f4f42				// "BOOT"
//
#define EBDA_BASE_ADDRESS 0x40E
//
// Error codes
//
#define	ACPI_SUCCESS		0
#define	ACPI_NO_RSDPTR		1
#define	ACPI_SIGNATURE		2
#define	ACPI_CHECKSUM		3
#define	ACPI_NULL_PTR		4
#define	ACPI_PROCREC_ERR	5
#define	ACPI_2_ERR			6
//
//
// Generic description header
//
#pragma pack(1)
typedef struct {        
    UINT32		Signature;			/* unique signature */
    UINT32		Length;				/* length of the table */
    UINT8		Revision;			/* current rev is 1 */
    UINT8		Checksum;			/* entire table must sum zero */
    UINT8		OEMId[6];			/* string that identifies OEM */
    UINT8		OEMTableId[8];		/* manufacturer model ID */
    UINT32		OEMRevision;		/* OEM revsion of this table */
    UINT8		CreatorId[4];		/* string that identifies creator */
    UINT32		CreatorRevision;	/* creator rev of this table */
} DESC_HEADER_t;
//
// RST_PTR
//
typedef struct {
	UINT64		Signature;			/* "RSD PTR " */
    UINT8       Checksum;			/* entire table must sum zero */
    UINT8       OemId[6];			/* string that identifies OEM */
    UINT8       Revision;
    UINT32      RSDTAddress;		/* physical address of the RSDT table */
    UINT32      Length;
    UINT64      XSDTAddress;		/* physical address of the XSDT table */
    UINT8       ExtendedChecksum;
    UINT8       Reserved[3];
} RSDP20_t;
//
// RSDT
//
typedef struct {
	DESC_HEADER_t Header;	/* header */
	UINT32 Entry[1];		/* array of physical address that point to other description */
} RSDTtbl_t;
//
// XSDT
//
typedef struct {
	DESC_HEADER_t Header;	/* header */
	UINT64 Entry[1];		/* array of physical address that point to other description */
} XSDTtbl_t;
//
// SLIC
//
typedef struct {
	UINT32 Ukn0;
	UINT32 Length;
	UINT32 Ukn1;
	UINT8 OemId[6];
	UINT8 OemTableId[8];
	UINT8 CreatorId[8];
	UINT32 Version;
	UINT64 Ukn2;
	UINT64 Ukn3;
	UINT8 Data[128];
} Marker_t;
//
typedef struct {
	UINT32 Ukn0;
	UINT32 Length;
	UINT32 Ukn1;
	UINT32 Ukn2;
	UINT8 Type[4];
	UINT32 Ukn4;
	UINT32 Ukn5;
	UINT8 Data[128];
} PublicKey_t;
//
typedef struct {
	DESC_HEADER_t Header;
	PublicKey_t PublicKey;
	Marker_t Marker;
} SLICtbl_t;
//
// MSDM
//
typedef struct {
	DESC_HEADER_t Header;
	UINT64 Ukn0;
	UINT64 Ukn1;
	UINT32 KeyLength;
	UINT8 Key[1];
} MSDMtbl_t;
//
#pragma pack()
#endif