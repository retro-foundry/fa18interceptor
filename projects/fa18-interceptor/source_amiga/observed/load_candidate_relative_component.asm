; Byte-exact candidate vector first-component load $C26FAE-$C26FB7.
; The negative-component handler at $C26FB8 remains raw.

                org     $C26FAE

load_candidate_relative_component:
                dc.w    $4CF0,$00E0,$0014 ; movem.l $14(a0,d0.w),d5-d7; retain original EA
                sub.l   d2,d5
                bge.b   $C26FBA
