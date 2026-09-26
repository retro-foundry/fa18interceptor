; Byte-exact observed mode-zero record-angle store route $C23FD4-$C23FF7.
; It stores d0 into record words +$6C/+6E, then gates the next route on bits 1
; and 3 of record byte +$01, clearing word-0 bit 0 on the bit-1 path.

                org     $C23FD4

store_mode_zero_record_angles:
                move.w  d0,$6C(a1)
                move.w  d0,$6E(a1)
                btst    #1,$1(a1)
                beq.w   $C243F2
                dc.w    $0269,$FFFE,$0000       ; andi.w #$FFFE,$0(a1)
                btst    #3,$1(a1)
                beq.w   $C243F2
                bra.b   $C24006
