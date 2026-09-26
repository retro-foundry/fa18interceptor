; Byte-exact observed C13D84 common record-update continuation $C146C2-$C146E3.
; It negates local -$18 into -$1C, then uses local -$32, the $20D0 bound, and
; shared flag bit 14 to select one of the adjacent continuation routes.

                org     $C146C2

RECORD_UPDATE_FLAGS             equ     $C458D2

continue_c13d84_record_update:
                move.w  -$18(a6),d0
                neg.w   d0
                move.w  d0,-$1C(a6)
                tst.w   -$32(a6)
                bne.b   $C14718
                cmpi.w  #$20D0,d0
                blt.b   $C14708
                move.w  RECORD_UPDATE_FLAGS.l,d0
                btst    #14,d0
                bne.b   $C14718
