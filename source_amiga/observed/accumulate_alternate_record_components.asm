; Byte-exact alternate helper $C1D0B6-$C1D10B.
; Observed callers provide D7 and D2/D4; field ownership remains structural.
                org $C1D0B6
ALT_COMPONENT_RECORD_BASE       equ $C46184
ALT_COMPONENT_ADJUSTMENT        equ $C45A66
ALT_COMPONENT_RESULT            equ $C45B3C
ALT_COMPONENT_READY             equ $C458BB

accumulate_alternate_record_components:
                lea ALT_COMPONENT_RECORD_BASE.l,a2
                move.w d7,d1
                move.w d7,d6
                andi.w #$ff00,d1
                add.w d1,d1
                adda.w d1,a2
                andi.w #$000f,d6
                move.l $14(a2),d3
                andi.l #$000fffff,d3
                asr.l d6,d3
                asl.l #8,d2
                add.l d3,d2
                move.l $1c(a2),d3
                andi.l #$000fffff,d3
                asr.l d6,d3
                asl.l #8,d4
                add.l d3,d4
                move.l $18(a2),d3
                move.l d3,d1
                asr.l d6,d3
                add.l ALT_COMPONENT_ADJUSTMENT.l,d1
                asr.l d6,d1
                move.l d1,ALT_COMPONENT_RESULT.l
                move.b #1,ALT_COMPONENT_READY.l
                rts
