;==============================================================================
; data section for sector 16 (superblock)
;==============================================================================
; if bootmger is on a partiton > 100mb there might be a superblock at the
; beginning of sector 16. so this space does not go to waste store some
; variables here
;==============================================================================

times 1E00h - ($-$$)	db 0		; align to the start of sector 16
times 80h		db 0		; superblock length 80h
times 2000h - ($-$$) - (DataEnd - DataBegin)	db 0
DataBegin:
ForceFallBack		db EBDA_IGNORED
ConfigByte		db DISABLE_BIOS_RW
AddOEMID		db ADD_OEMID_TO_TABLES
VersionByte		db 2

;------------------------------------------------------------------------------
; Include the SLIC (a 374-byte binary file)
;------------------------------------------------------------------------------
SLIC.addr_includedat:
file '..\SLIC\dummy.bin'
DataEnd:
;==============================================================================