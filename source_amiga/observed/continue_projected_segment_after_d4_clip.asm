; Byte-exact C2EF54-C2EFBD continuation after the D4 clipping branches.
; It selects complementary clip helpers before joining the D3 branch at C2EFBE.
                org     $C2EF54
PROJECT_SEGMENT_STATUS_WORD         equ     $C45ACA
REJECT_PROJECTED_SEGMENT            equ     $C2EE44
PROJECT_AND_SUBMIT_SEGMENT          equ     $C2EF0E
SUBMIT_PROJECTED_SEGMENT            equ     $C2F03A
PROJECT_SEGMENT_REJECT_PATH         equ     $C2F030
PROJECTED_SEGMENT_D3_CONTINUATION   equ     $C2EFBE
CLIP_NONNEGATIVE_D4_FIRST           equ     $C2F128
CLIP_NONNEGATIVE_D4_SECOND          equ     $C2F156

continue_projected_segment_after_d4_clip:
                bra.w   SUBMIT_PROJECTED_SEGMENT
                cmp.w   d5,d4
                blt.b   clip_negative_d4_second_half
                move.w  8(a1),d2
                move.w  10(a1),d6
                cmp.w   d2,d6
                ble.w   REJECT_PROJECTED_SEGMENT
                bsr.w   CLIP_NONNEGATIVE_D4_FIRST
                beq.b   PROJECT_AND_SUBMIT_SEGMENT
                tst.w   PROJECT_SEGMENT_STATUS_WORD.l
                bge.b   PROJECTED_SEGMENT_D3_CONTINUATION
                neg.w   d2
                cmp.w   d2,d6
                ble.b   PROJECTED_SEGMENT_D3_CONTINUATION
                bsr.w   CLIP_NONNEGATIVE_D4_SECOND
                bne.b   PROJECTED_SEGMENT_D3_CONTINUATION
                bra.w   SUBMIT_PROJECTED_SEGMENT
clip_negative_d4_second_half:
                move.w  d4,d6
                neg.w   d6
                cmp.w   d5,d6
                blt.w   PROJECT_SEGMENT_REJECT_PATH
                move.w  8(a1),d2
                move.w  10(a1),d6
                neg.w   d2
                cmp.w   d2,d6
                ble.w   REJECT_PROJECTED_SEGMENT
                bsr.w   CLIP_NONNEGATIVE_D4_SECOND
                beq.b   continue_projected_segment_after_d4_clip
                tst.w   PROJECT_SEGMENT_STATUS_WORD.l
                bge.b   PROJECTED_SEGMENT_D3_CONTINUATION
                neg.w   d2
                cmp.w   d2,d6
                ble.b   PROJECTED_SEGMENT_D3_CONTINUATION
                bsr.w   CLIP_NONNEGATIVE_D4_FIRST
                beq.w   PROJECT_AND_SUBMIT_SEGMENT
