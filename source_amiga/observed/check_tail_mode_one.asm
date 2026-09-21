; Byte-exact observed early-return prefix $C2B3C0-$C2B3CB.
; The training parent calls the entry at $C2B3C2.

                org     $C2B3C0

TAIL_MODE                       equ $C458A6

tail_mode_early_return:
                rts

check_tail_mode_one:
                cmpi.b  #1,TAIL_MODE.l
                bne.s   tail_mode_early_return
