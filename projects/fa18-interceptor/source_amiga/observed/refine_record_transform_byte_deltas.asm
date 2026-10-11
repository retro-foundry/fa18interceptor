; Byte-exact observed record-transform byte-delta refinement $C1ECFC-$C1ED29.
; Two low-byte global values are offset by frame-local words, promoted to
; fixed-point longs, divided by four, and accumulated into d2 and d4.

                org     $C1ECFC

REFINE_COMPONENT_X_SOURCE       equ     $C4594C
REFINE_COMPONENT_Y_SOURCE       equ     $C4594E

refine_record_transform_byte_deltas:
                moveq   #0,d1
                move.w  REFINE_COMPONENT_X_SOURCE.l,d1
                andi.w  #$00FF,d1
                sub.w   -$1C(a6),d1
                swap    d1
                asr.l   #2,d1
                add.l   d1,d2
                moveq   #0,d1
                move.w  REFINE_COMPONENT_Y_SOURCE.l,d1
                andi.w  #$00FF,d1
                sub.w   -$1E(a6),d1
                swap    d1
                asr.l   #2,d1
                add.l   d1,d4
                rts
