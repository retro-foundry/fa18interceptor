; Byte-exact observed scaling block in $C123FA, $C124EE-$C12517.

                org     $C124EE

prepare_c123fa_scaled_components:
                move.w  #8,-$18(a6)
                move.w  #6,-$1a(a6)
                move.w  -$18(a6),d0
                ext.l   d0
                move.l  $10(a6),d1
                asr.l   d0,d1
                move.l  $18(a6),d2
                asr.l   d0,d2
                move.w  d1,-$4(a6)
                move.w  d2,-$6(a6)
                cmp.w   d2,d1
                bgt.b   $C1256A
