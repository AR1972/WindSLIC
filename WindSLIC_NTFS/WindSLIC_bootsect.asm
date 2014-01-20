use16
file 'VBS_part1.bin'
call injector
mov cx, 2000h - windslic
mov di, windslic
erace:
mov byte [di], byte 00h
inc di
loop erace
times 27Ah - ($-$$) db 90h
file 'VBS_part2.bin'
windslic:
enc_key  dq ENCRYPT_KEY
enc_key_len:
injector:
include 'windslic.asm'
times 2000h - ($ - $$)	db 0  ; pad to 8192b
