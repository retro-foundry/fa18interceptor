; Byte-exact C2EF12-C2EF53 nonnegative-D4 clipping branch of C2EE4A.
                org     $C2EF12
PROJECT_SEGMENT_STATUS_WORD         equ     $C45ACA
REJECT_PROJECTED_SEGMENT            equ     $C2EE44
PROJECT_AND_SUBMIT_SEGMENT          equ     $C2EF0E
CLIP_NONNEGATIVE_D4_FIRST           equ     $C2F128
CLIP_NONNEGATIVE_D4_SECOND          equ     $C2F156
CONTINUE_NONNEGATIVE_D4_RESULT      equ     $C2EF54
prepare_projected_segment_nonnegative_d4:
                cmp.w   d5,d4
                blt.w   REJECT_PROJECTED_SEGMENT
                move.w  8(a1),d2
                move.w  10(a1),d6
                cmp.w   d2,d6
                ble.w   REJECT_PROJECTED_SEGMENT
                bsr.w   CLIP_NONNEGATIVE_D4_FIRST
                beq.b   PROJECT_AND_SUBMIT_SEGMENT
                tst.w   PROJECT_SEGMENT_STATUS_WORD.l
                bge.w   REJECT_PROJECTED_SEGMENT
                move.w  d4,d6
                neg.w   d6
                cmp.w   d5,d6
                blt.w   REJECT_PROJECTED_SEGMENT
                move.w  10(a1),d6
                neg.w   d2
                cmp.w   d2,d6
                ble.w   REJECT_PROJECTED_SEGMENT
                bsr.w   CLIP_NONNEGATIVE_D4_SECOND
                bne.w   REJECT_PROJECTED_SEGMENT
