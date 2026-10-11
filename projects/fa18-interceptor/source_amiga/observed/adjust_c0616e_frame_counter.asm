; Byte-exact observed frame-counter adjustment helper $C0616E-$C06177.

                org     $C0616E

adjust_c0616e_frame_counter:
                ; Preserve the captured ADDI.W immediate encoding.
                dc.w    $066e,$0001,$0020
                move.l  a6,d0
                rts
