;==============================================================================
; data section for sector 16 (superblock)
;==============================================================================
; if bootmger is on a partiton > 100mb there might be a superblock at the
; beginning of sector 16. so this space does not go to waste store some
; variables here
;==============================================================================

times 1E00h - ($-$$)	db 0		; align to the start of sector 16
dword_222		dd 0
dword_226		dd 0
dword_22A		dd 0
dword_22E		dd 0
dword_232		dd 0
dword_236		dd 0
dword_23A		dd 0
dword_23E		dd 0
dword_246		dd 0
dword_24A		dd 0
dword_24E		dd 0
cluster_size_bytes	dd 0
record_size_bytes	dd 0
dword_26A		dd 0
dword_26E		dd 0
dword_272		dd 0
dword_276		dd 0
sectors_per_record	dd 0
times 7Dh-3Fh		db 0

ForceFallBack		db EBDA_IGNORED
ConfigByte		db DISABLE_BIOS_RW
AddOEMID		db ADD_OEMID_TO_TABLES
VersionByte		db 2

;------------------------------------------------------------------------------
; Include the SLIC (a 374-byte binary file)
;------------------------------------------------------------------------------
SLIC.addr_includedat:
file '..\SLIC\dummy.bin'
;==============================================================================