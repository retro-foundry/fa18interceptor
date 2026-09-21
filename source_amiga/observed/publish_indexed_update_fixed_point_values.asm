; Byte-exact fixed-point normalization and local publication $C25ED2-$C25F01.

                org     $C25ED2

publish_indexed_update_fixed_point_values:
                asr.l   #8,d2
                asr.l   #8,d3
                asr.l   #8,d4
                move.w  d2,$0C(a1)
                move.w  d4,$0E(a1)
                move.l  d3,$10(a1)
                swap    d2
                rol.l   #4,d2
                swap    d4
                rol.l   #4,d4
                move.w  d2,d5
                move.w  d4,d7
                subq.w  #3,d2
                neg.w   d2
                subq.w  #3,d4
                neg.w   d4
                add.w   d4,d4
                add.w   d4,d4
                add.w   d2,d4
                cmp.b   $0A(a1),d4
                dc.w    $674A                   ; beq.b $C25F4E
