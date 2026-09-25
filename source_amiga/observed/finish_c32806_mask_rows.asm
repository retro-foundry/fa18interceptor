; Byte-exact zero-selector and epilogue tail $C32880-$C328A5.

                org     $C32880

finish_c32806_mask_rows:
.zero_selector_rows:
                move.l  #$E0000000,d1
                lsr.l   d2,d1
                move.l  (a3),d3
                not.l   d1
                and.l   d3,d1
                move.l  d1,(a3)
                addq.w  #1,a0
                ; Preserve the original ADDA.W immediate encoding.
                dc.w    $D6FC,$0028
                dbra    d6,.zero_selector_rows
                swap    d2
                move.w  d2,d3
                swap    d6
                movem.l (a7)+,d0-d1/d6
                rts
