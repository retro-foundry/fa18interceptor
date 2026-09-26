; Byte-exact observed C13D84 local-word flag adjustment $C141F4-$C1421B.
; Active-record byte +$7C controls a three-quarter adjustment to local -$16;
; a separate local record's bit 11 then selects the following route.

                org     $C141F4

CURRENT_CONTROL_RECORD          equ     $C18210

adjust_c13d84_local_word_by_record_flags:
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.b  $7C(a0),d0
                andi.b  #$70,d0
                tst.b   d0
                bne.b   $C14210
                move.w  -$16(a6),d0
                asr.w   #2,d0
                sub.w   d0,-$16(a6)
                movea.l -$2C(a6),a0
                move.w  (a0),d0
                btst    #11,d0
                beq.b   $C14226
