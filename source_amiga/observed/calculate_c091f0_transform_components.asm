; Byte-exact observed transform component calculation $C091F0-$C09249.

                org     $C091F0

calculate_c091f0_transform_components:
                lea     (a1),a2
                ; ADDA.W #$0092,A2; preserve the observed immediate encoding.
                dc.w    $d4fc,$0092
                move.w  d3,d6
                move.w  d4,d0
                move.w  d5,d7
                muls.w  (a2),d6
                muls.w  2(a2),d0
                muls.w  4(a2),d7
                add.l   d6,d0
                add.l   d7,d0
                move.w  d3,d6
                move.w  d4,d1
                move.w  d5,d7
                muls.w  6(a2),d6
                muls.w  8(a2),d1
                muls.w  $a(a2),d7
                add.l   d6,d1
                add.l   d7,d1
                move.w  d4,d2
                muls.w  $c(a2),d3
                muls.w  $e(a2),d2
                muls.w  $10(a2),d5
                add.l   d3,d2
                add.l   d5,d2
                asr.l   #4,d0
                asr.l   #4,d1
                asr.l   #4,d2
                add.l   $14(a1),d0
                add.l   $18(a1),d1
                add.l   $1c(a1),d2
                movem.l (sp)+,a1-a2
                rts
