;ws_gdtrs.inc - The GDTR table used for protected-mode/unreal-mode setup
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
;Global Descriptor Table Stuff
;-----------------------------------------------------------------------
;// memory model: segmentation.
;// create and load GDT.
;// two segments: code and data.
;// 
;// segment registers      descriptor table(GDT and LDT)
;// +----------------+    +--------------+-------+--------+
;// | Selector       | -> | Base address | Limit | Access |
;// +----------------+    +--------------+-------+--------+
;//
;//
;// using DS register to select a descriptor from GDT.
;//    segment registers      descriptor table(GDT and LDT)
;//    +----------------+    +--------------+-------+--------+
;// DS | Selector       | -> | Base address | Limit | Access |
;//    +----------------+    +--------------+-------+--------+
;// for example:
;//                 GDT 
;//                 +------------------------+   +--------+   |...        ...| FF,FF,FF
;//                 |                        |   | offset |   |              |
;//                 |                        |   +--------+   +--------------+ 10,00,FF
;//                 |                        |       |        |              |
;//                 |                        |      ¨<92>---->| Data segment |
;//    +------+     +--+-----+-+--------+----+       |        |              |
;// DS | 0008 | ->  |00| ... |0|10,00,00|00FF|(1) ----------->+--------------+ 10,00,00
;//    +------+     +--+-----+-+--------+----+                |              |
;//                 | null                   |(0)             |              |
;//                 +------------------------+                |...        ...| 00,00,00
;// base address: 00,10,00,00   
;// limit:        0,00FF
;// DS value = 0008, 0008>>3 = 0001, so the (1)-GD in GDT is selected.    
;//
;// code and data segment descriptor (4 double word, each is 16 bits)   
;// +---------------------------------------+-----+-----+-----+------+--------------+ 
;// | Base(31-24)                           |G(23)|D(22)|0(21)|AV(20)| Limit(19-16) | 
;// +---------------------------------------+-----+-----+-----+------+--------------+ 
;// | Access rights(31-24)                  | Base(23-16)                           |
;// +---------------------------------------+---------------------------------------+
;// | Base(15-0)                                                                    | 
;// +-------------------------------------------------------------------------------+
;// | Limit(15-0)                                                                   |
;// +-------------------------------------------------------------------------------+
;//
;// Access rights: (8 bits)
;// +-----+----------+-----+-----+--------+-------+-----+
;// |P(31)|DPL(30-29)|S(28)|E(27)|ED/C(26)|R/W(25)|A(24)|
;// +-----+----------+-----+-----+--------+-------+-----+
;// [bit 24]    A = 0: Segment not accessed.                                          
;//             A = 1: Segment has been accessed.
;// [bit 25]    R/W = 1: data may be written.
;//             R/W = 0: data may not be written.
;// [bit 26]    set it to 0. (·ÇË³´ÓÐÔ)
;// [bit 27]    E = 0: descriptor describes a data segment.
;//             E = 1: descriptor describes a code segment.  
;// [bit 28]    S = 0£ºsystem descriptor.
;//             S = 1: code or segment descriptor. 
;// [bit 29-30] DPL: sets the descriptor privilege level.
;//             DPL = 00: the highest privilege level, used by OS.
;//             DPL = 11: the lowest privilege level, used by application in user-mode.
;// [bit 31]    P = 0: descriptor is undefined
;//             P = 1: segment contains a valid base and limit  
;
;----------------------------- hilimit ---------------------------------
;
;                             Sys  Sgg  G x 0 x               0
;                             Code Seg                        1 1 C R A
;                             Data seg  G B                   1 0 E W A
;                            +-------------+-/-/-/-/--------+-/--/-/-------+------------+
;                            |             | |D| |A|Seg-Lim | |D | |       |            |
;                  hilimit = | base 31:24  |G|/|0|V|        |P| P|S| Type  | Base 23:16 |
;                            |             | |B| |L| 19:16  | |L | |       |            |
;                            +-------------+-\-\-\-\--------+-\--\-\-------+------------+
;
;
;               AVL  -- Available for use by system software
;               BASE -- Segment base address
;               D/B  -- Default operation size
;                       (0 = 16-bit segment; 1 = 32-bit segment)

;               DPL  -- Descriptor privilege level (0-3)
;               G    -- Granularity
;                       (clear for byte unit of seg. limit, set for 4k unit)
;
;               LIMIT-- Segment Limit
;               P    -- Segment present
;               S    -- Descriptor type
;                       (0 = system; 1 = code or data)
;
;-----------------------------------------------------------------------
;-----------------------------------------------------------------------
GDTR:
;-----------------------------------------------------------------------
;startp "Global Descriptor Table (GDTR)"
;-----------------------------------------------------------------------
	dw	GDTR.GDT1_END-GDTR.GDT1-1h;8*3-1;limit GDT length
	dd	00000000h
GDTR.GDT1:
	dw	0		; limit (bit0-15)
	dw	0		; base address (bit0-15)
	db	0		; hibase address (bit16-23)
	db	0		; access (attribute)
	db	0		; hilimit (bit16-19,20-23)
	db	0		; msbase address (bit24-31)
GDTR.DATA64K:			; cs - prom code segment
GDTR.DATA64K_INDEX  = GDTR.DATA64K - GDTR.GDT1
	dw	  0ffffh	; limit
	dw	  00000h	; base
	db	  000h		; hibase
	db	  093h		; access  
	db	  000h		; hilimit D/B Must set to 0 ,or error occur!
	db	  000h		; msbase
GDTR.DATA4G:
GDTR.DATA4G_INDEX   = GDTR.DATA4G - GDTR.GDT1
	dw	  0ffffh	; limit
	dw	  00000h	; base data segment points to
	db	  000h		; hibase        ; 00000000
	db	  093h		; access
	db	  08fh		; hilimit (4GB)
	db	  000h		; msbase
GDTR.GDT1_END:
;-----------------------------------------------------------------------
;endofp "Global Descriptor Table (GDTR)"
;-----------------------------------------------------------------------



;-----------------------------------------------------------------------
;eof - ws_gdtrs.inc
