; Byte-exact observed candidate-record component-bound checks $C27974-$C279B3.
; This path accepts the candidate only for type $10, a zero low input nibble,
; and three shifted components which do not fall below the accumulated d2 term.

                org     $C27974

check_candidate_shifted_component_bounds:
                move.b  $62(a3),d7
                andi.b  #$F0,d7
                cmpi.b  #$10,d7
                bne.b   $C279B8
                andi.w  #$000F,d0
                bne.b   $C279B8
                move.w  $A6(a3),d1
                move.b  $7D(a3),d3
                andi.w  #$000F,d3
                asr.w   d3,d1
                ext.l   d1
                add.l   d2,d1
                blt.b   $C279C2
                move.w  $AC(a3),d1
                asr.w   d3,d1
                ext.l   d1
                add.l   d2,d1
                blt.b   $C279C2
                move.w  $B2(a3),d1
                asr.w   d3,d1
                ext.l   d1
                add.l   d2,d1
                bra.b   $C279B8
