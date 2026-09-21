; Byte-exact indexed-update fixed-point preparation $C25E86-$C25EB1.

                org     $C25E86

prepare_indexed_update_fixed_point_values:
                movem.l d5-d7/a1,-(a7)
                move.l  $18(a1),d3
                moveq   #0,d6
                move.l  $14(a1),d0
                move.l  d0,d2
                andi.l  #$003FFFFF,d2
                move.l  $1C(a1),d1
                move.l  d1,d4
                andi.l  #$003FFFFF,d4
                swap    d0
                asr.w   #6,d0
                cmp.w   $06(a1),d0
                dc.w    $6706                   ; beq.b $C25EB8
