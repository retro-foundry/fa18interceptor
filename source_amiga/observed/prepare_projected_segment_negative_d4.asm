; Byte-exact C2EECA-C2EF11 negative-D4 clipping branch of C2EE4A.
; Preserves the raw global-status gate and established intersection helpers.
                org     $C2EECA
PROJECT_SEGMENT_STATUS_WORD        equ     $C45ACA
REJECT_PROJECTED_SEGMENT           equ     $C2EE44
CONTINUE_NEGATIVE_D4_INTERSECTION  equ     $C2EF12
CLIP_NEGATIVE_D4_FIRST             equ     $C2F156
CLIP_NEGATIVE_D4_SECOND            equ     $C2F128
CONTINUE_NEGATIVE_D4_RESULT        equ     $C2EF54
PROJECT_AND_SUBMIT_SEGMENT         equ     $C2F03A
prepare_projected_segment_negative_d4:
                tst.w   d4
                bge.b   CONTINUE_NEGATIVE_D4_INTERSECTION
                move.w  d4,d6
                neg.w   d6
                cmp.w   d5,d6
                blt.w   REJECT_PROJECTED_SEGMENT
                move.w  8(a1),d2
                move.w  10(a1),d6
                neg.w   d2
                cmp.w   d2,d6
                ble.w   REJECT_PROJECTED_SEGMENT
                bsr.w   CLIP_NEGATIVE_D4_FIRST
                beq.b   CONTINUE_NEGATIVE_D4_RESULT
                tst.w   PROJECT_SEGMENT_STATUS_WORD.l
                bge.w   REJECT_PROJECTED_SEGMENT
                cmp.w   d5,d4
                blt.w   REJECT_PROJECTED_SEGMENT
                neg.w   d2
                cmp.w   d2,d6
                ble.w   REJECT_PROJECTED_SEGMENT
                bsr.w   CLIP_NEGATIVE_D4_SECOND
                bne.w   REJECT_PROJECTED_SEGMENT
                bra.w   PROJECT_AND_SUBMIT_SEGMENT
