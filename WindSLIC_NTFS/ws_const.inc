
;Constants
;-----------------------------------------------------------------------
;-----------------------------------------------------------------------
; -v- BOOTSPLASH_VERBOSITY Levels
;-----------------------------------------------------------------------
; These levels are just constants used with the:
;       -write_out
;       -write_out_crlf
;       -disp_str
;       -disp_str_crlf
;       -disp_dword_hex, disp_word_hex, disp_byte_hex
;       -disp_dword_hex_crlf, disp_word_hex_crlf, disp_byte_hex_crlf
; macros.
;
; They can be used as levels when calling those macros to
; suppress display unless BOOTSPLASH_VERBOSITY (above) is set to a higher
; level than the level.
; An Example call: writeoutcrlf "This is an example",BS_BASIC
; Another Example call: dispdwordhex eax,BS_DETAILED
;
; NOTE: These should not need to be changed here. The setting choice
; is made from the main file, not here.
;

	BS_HIDDEN	=	0	; use this level to hide BootSplash unless BS_VISIBLE is 1
	BS_MINIMAL	=	1	; use this level to display if Bootsplash is even enabled
	BS_BASIC	=	2	; use this level for basic progress messages
	BS_DETAILED	=	3	; use this level for detailed progress messsages
	BS_DEBUG	=	4	; use this level for debug progress messsages
	;---------------
	BS_DEFAULT	=	BS_HIDDEN	; By Default, Choose Hidden
;
;-----------------------------------------------------------------------
; -^- BOOTSPLASH_VERBOSITY Levels
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
; -v- ACPI Tables
;-----------------------------------------------------------------------
;-----------------------------------------------------------------------
; -v- RSDP Table
;-----------------------------------------------------------------------
	RSDP_val_Signature = 'RSD PTR '
	RSDP_ofs_Signature = 0
	RSDP_len_Signature = 8
	;
	RSDP_ofs_Checksum = 8
	RSDP_len_Checksum = 1
	;
	RSDP_ofs_OEMID = 9
	RSDP_len_OEMID = 6
	;
	RSDP_ofs_Revision = 15
	RSDP_len_Revision = 1
	;
	RSDP_ofs_RSDTAddress = 16
	RSDP_len_RSDTAddress = 4
	;
	RSDP_ofs_Length = 20
	RSDP_len_Length = 4
	;
	RSDP_ofs_XSDTAddress = 24
	RSDP_len_XSDTAddress = 8
	;
	RSDP_ofs_ExtdChecksum = 32
	RSDP_len_ExtdChecksum = 1
	;
	RSDP_ofs_Rsvd1 = 33
	RSDP_len_Rsvd1 = 3
;-----------------------------------------------------------------------
; -^- RSDP Table
;-----------------------------------------------------------------------
;-----------------------------------------------------------------------
; -v- RSDT Table
;-----------------------------------------------------------------------
	RSDT_val_Signature = 'RSDT'
	RSDT_ofs_Signature = 0
	RSDT_len_Signature = 4
	;
	RSDT_ofs_Length = 4
	RSDT_len_Length = 4
	;
	RSDT_ofs_Revision = 8
	RSDT_len_Revision = 1
	;
	RSDT_ofs_Checksum = 9
	RSDT_len_Checksum = 1
	;
	RSDT_ofs_OEMID = 10
	RSDT_len_OEMID = 6
	;
	RSDT_ofs_OEMTableID = 16
	RSDT_len_OEMTableID = 8
	;
	RSDT_ofs_OEMRevision = 24
	RSDT_len_OEMRevision = 4
	;
	RSDT_ofs_CreatorID = 28
	RSDT_len_CreatorID = 4
	;
	RSDT_ofs_CreatorRevision = 32
	RSDT_len_CreatorRevision = 4
	;
	RSDT_ofs_EntryTable = 36
	RSDT_len_EntryTableEntry = 4	;#This is the length *per* entry in the table!
;-----------------------------------------------------------------------
; -^- RSDT Table
;-----------------------------------------------------------------------
;-----------------------------------------------------------------------
; -v- XSDT Table
;-----------------------------------------------------------------------
	XSDT_val_Signature = 'XSDT'
	XSDT_ofs_Signature = 0
	XSDT_len_Signature = 4
	;
	XSDT_ofs_Length = 4
	XSDT_len_Length = 4
	;
	XSDT_ofs_Revision = 8
	XSDT_len_Revision = 1
	;
	XSDT_ofs_Checksum = 9
	XSDT_len_Checksum = 1
	;
	XSDT_ofs_OEMID = 10
	XSDT_len_OEMID = 6
	;
	XSDT_ofs_OEMTableID = 16
	XSDT_len_OEMTableID = 8
	;
	XSDT_ofs_OEMRevision = 24
	XSDT_len_OEMRevision = 4
	;
	XSDT_ofs_CreatorID = 28
	XSDT_len_CreatorID = 4
	;
	XSDT_ofs_CreatorRevision = 32
	XSDT_len_CreatorRevision = 4
	;
	XSDT_ofs_EntryTable = 36
	XSDT_len_EntryTableEntry = 8	;#This is the length *per* entry in the table!
;-----------------------------------------------------------------------
; -^- XSDT Table
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
; -v- SLIC Table
;-----------------------------------------------------------------------
	SLIC_val_Signature = 'SLIC'
	;
	SLIC_ofs_Signature = 0
	SLIC_len_Signature = 4
	;
	SLIC_ofs_Length = 4
	SLIC_len_Length = 4
	;
	SLIC_ofs_Revision = 8
	SLIC_len_Revision = 1
	;
	SLIC_ofs_Checksum = 9
	SLIC_len_Checksum = 1
	;
	SLIC_ofs_OEMID = 10
	SLIC_len_OEMID = 6
	;
	SLIC_ofs_OEMTableID = 16
	SLIC_len_OEMTableID = 8
	;
	SLIC_ofs_OEMRevision = 24
	SLIC_len_OEMRevision = 4
	;
	SLIC_ofs_CreatorID = 28
	SLIC_len_CreatorID = 4
	;
	SLIC_ofs_CreatorRevision = 32
	SLIC_len_CreatorRevision = 4
;-----------------------------------------------------------------------
; -^- SLIC Table
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
; -v- Generic ACPI Tables
;-----------------------------------------------------------------------
	ACPIGenericTable_ofs_Signature = 0
	ACPIGenericTable_len_Signature = 4
	;
	ACPIGenericTable_ofs_Length = 4
	ACPIGenericTable_len_Length = 4
	;
	ACPIGenericTable_ofs_Revision = 8
	ACPIGenericTable_len_Revision = 1
	;
	ACPIGenericTable_ofs_Checksum = 9
	ACPIGenericTable_len_Checksum = 1
	;
	ACPIGenericTable_ofs_OEMID = 10
	ACPIGenericTable_len_OEMID = 6
	;
	ACPIGenericTable_ofs_OEMTableID = 16
	ACPIGenericTable_len_OEMTableID = 8
	;
	ACPIGenericTable_ofs_OEMRevision = 24
	ACPIGenericTable_len_OEMRevision = 4
	;
	ACPIGenericTable_ofs_CreatorID = 28
	ACPIGenericTable_len_CreatorID = 4
	;
	ACPIGenericTable_ofs_CreatorRevision = 32
	ACPIGenericTable_len_CreatorRevision = 4
;-----------------------------------------------------------------------
; -^- Generic ACPI Tables
;-----------------------------------------------------------------------
;-----------------------------------------------------------------------
; -^- ACPI Tables
;-----------------------------------------------------------------------

;----------------------------------------------------------------------- 
; -v- E820 Buffer
;----------------------------------------------------------------------- 
E820Struct_len_Struct	       = 0x28 
E820Struct_len_Buffer	       = 0x14 
 
E820Struct_ofs_Start		   = 0x0000	   ;offset to temp e820 buffer in "ds" 
 
E820Struct_ofs_BaseAddrLow	   = 0x00000000 
E820Struct_ofs_BaseAddrHigh	   = 0x00000004 
E820Struct_ofs_LengthLow	   = 0x00000008 
E820Struct_ofs_LengthHigh	   = 0x0000000C 
E820Struct_ofs_Type		   = 0x00000010 
E820Struct_ofs_ExtdAttrs	   = 0x00000014 
E820Struct_ofs_EndAddrLow	   = 0x00000018 
E820Struct_ofs_EndAddrHigh	   = 0x0000001C 
E820Struct_ofs_ContCodeForThis	   = 0x00000020 
E820Struct_ofs_ContCodeForNext	   = 0x00000024 
 
;----------------------------------------------------------------------- 
; -^- ; E820 Buffer
;----------------------------------------------------------------------- 




;#eof - ws_const.inc
