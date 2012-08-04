;-----------------------------------------------------------------------
; -v- Meta-Macro Definitions -v-
;-----------------------------------------------------------------------
; Meta Macros are defined as macros that do not affect the 
; actual target that is assembled.
;
; They tend to be macros that display data to stdout
; during the assembly process, 
; or emit debugging information that can be captured
; to aid with the debugging/virtual-testing process.
;-----------------------------------------------------------------------

;----------------------------------------------------------
;The DEBUG_CODESEG is the expected QEMU code-segment where this ROM
; will be loaded.  It doesn't have any effect outside of QEMU dev/testing.
; QEMU usually loads the option-rom at segment:offset 0xc900:0x0000
;	which is linear address (0xc9000)
;----------------------------------------------------------
;NOTE: This should probably be defined in the main asm file.
;It is included (commented out) here for reference only.
;----------------------------------------------------------
;DEBUG_CODESEG = 0xc900
;----------------------------------------------------------

;----------------------------------------------------------
; -v- [META] macro meta_display_offset tag,arg1
;----------------------------------------------------------
;	where	: tag = informative name to display
;		: arg1 is nothing, "b", or "t"
;		: 	with "b" causing breakpoint
;		: 	with "t" causing temporary breakpoint
;----------------------------------------------------------
;Purpose: Debugging Aid. 
; Displays the tag and offset to stdout in a fashion
; that can be captured to a file, filtered on, and
; be used as as a command file for GDB to cause it
; to break at predefined locations.
;----------------------------------------------------------
macro meta_display_offset tag,arg1
{
	if (METADEBUG_ENABLED = 1)
		address = ((DEBUG_CODESEG * 16) + $)
		bits = 16
		meta_display_debugtag
		display "#"
		display `tag
		display " == "
		meta_display_val_as_hex $
		display 13,10
		if arg1 eq 't'
			meta_display_debugtag
			display "tbreak *"
			meta_display_val_as_hex address,20
			display 13,10
		else if arg1 eq 'b'
			meta_display_debugtag
			display "break *"
			meta_display_val_as_hex address,20
			display 13,10
		end if
	end if
}
;----------------------------------------------------------

;----------------------------------------------------------
; -v- [META] macro bp tag
;----------------------------------------------------------
;	where	: tag = informative name to display
;----------------------------------------------------------
;Purpose: Debugging Aid.
; Emits a GDB-syntax breakpoint in the form of a
; tag (tag) and the current assembly offset ($)
; to stdout.
; It does its work by calling meta_display_offset with
; the tag and a "b" value, requesting that it be 
; turned into a GDB-syntax breakpoint directive.
;----------------------------------------------------------
macro bp tag
{
	if (METADEBUG_ENABLED = 1)
		meta_display_offset tag,"b"
	end if
	
}
;----------------------------------------------------------

;----------------------------------------------------------
; -v- [META] macro tbp tag
;----------------------------------------------------------
;	where	: tag = informative name to display
;----------------------------------------------------------
;Purpose: Debugging Aid.
; Emits a GDB-syntax temp-breakpt in the form of a
; tag (tag) and the current assembly offset ($)
; to stdout.
; It does its work by calling meta_display_offset with
; the tag and a "t" value, requesting that it be 
; turned into a GDB-syntax temporary-breakpoint directive.
;----------------------------------------------------------
macro tbp tag
{
	if (METADEBUG_ENABLED = 1)
		meta_display_offset tag,"t"
	end if
	
}
;----------------------------------------------------------

;----------------------------------------------------------
; -v- [META] macro op tag
;----------------------------------------------------------
;	where	: tag = informative name to display
;----------------------------------------------------------
;Purpose: Debugging Aid. Emits a tag (tag) and the
; current assembly offset ($) to stdout.
;----------------------------------------------------------
macro op tag
{
	if (METADEBUG_ENABLED = 1)
		meta_display_offset tag
	end if
	
}
;----------------------------------------------------------

;----------------------------------------------------------
; -v- [META] macro startp tag
;----------------------------------------------------------
;       where   : tag = informative name to display
;----------------------------------------------------------
;Purpose: Debugging Aid. Emits a tag (tag) and the
; current assembly offset ($) to stdout.
;----------------------------------------------------------
macro startp tag
{
        if (METADEBUG_ENABLED = 1)
                STARTP_MARKER = $
                meta_display_debugtag
                display "#      -v- "
                display `tag
                display " == "
                meta_display_val_as_hex $
                display 13,10
        end if

}
;----------------------------------------------------------
;----------------------------------------------------------
; -v- [META] macro endofp tag
;----------------------------------------------------------
;       where   : tag = informative name to display
;----------------------------------------------------------
;Purpose: Debugging Aid. Emits a tag (tag) and the
; current assembly offset ($) to stdout.
;----------------------------------------------------------
macro endofp tag
{
        if (METADEBUG_ENABLED = 1)
                ENDOFP_MARKER = $

                meta_display_debugtag
                display "#      -^- "
                display `tag
                display " == "
                meta_display_val_as_hex $
                display 13,10

                meta_display_debugtag
                display "#"
                meta_display_val_as_hex (ENDOFP_MARKER - STARTP_MARKER)
                display '  [lengthof: '
                display `tag
                display ']'
                display 13,10
        end if

}
;----------------------------------------------------------



;----------------------------------------------------------
; -v- [META] macro meta_display_val_as_hex value,lenbits
;----------------------------------------------------------
;	where	: value = numeric value 
;		: lenbits is nothing, "16", or "20"
;		: 	with "16" causing 4-digit display
;		: 	with "20" causing 5-digit display
;----------------------------------------------------------
;Purpose: The fasm display directive is relatively weak.
; This macro (originally provided in the FASM manual)
; Parses a large number into its Hex equivalent quoted
; string, and displays the result.
;----------------------------------------------------------
macro meta_display_val_as_hex value,lenbits
{
	if ((lenbits eq 16) | (lenbits eq 20) | (lenbits eq 32) | (lenbits eq 64))
		bits = lenbits
	else
		bits = 16
	end if
	display '0x'
	repeat bits/4
        d = '0' + value shr (bits-%*4) and 0Fh
		if d > '9'
			d = d + 'A'-'9'-1
		end if
		display d
	end repeat
}
;----------------------------------------------------------

;----------------------------------------------------------
; -v- [META] macro meta_display_debugtag
;----------------------------------------------------------
;Purpose: Debugging Aid.
; Prints a specially formatted tag string to stdout.
; This helps with capturing stdout and using it to
; feed other programs to help with debugging.
;----------------------------------------------------------
macro meta_display_debugtag
{
	if (METADEBUG_ENABLED = 1)
		display "#[debug]	"
	end if
}
;----------------------------------------------------------


;-----------------------------------------------------------------------
; -^- Meta-Macro Definitions -^-
;-----------------------------------------------------------------------

