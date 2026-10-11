; Byte-exact observed C500 fallback tail $C50134-$C50157.
; It initializes the fallback record words, applies the descriptor's DMACON
; word, delays for 400 decrement iterations, then restores the caller set.

                org     $C50134

CUSTOM_DMACON                   equ     $DFF096

finish_c500_blitter_record_fallback:
                move.w  #0,8(a0)
                move.w  #$7C,6(a0)
                move.w  $12(a1),CUSTOM_DMACON
                move.w  #$190,d0
.delay:
                dc.w    $0440,$0001             ; subi.w #1,d0
                bne.b   .delay
                movem.l (a7)+,d0-d3/a0-a4
                rts
