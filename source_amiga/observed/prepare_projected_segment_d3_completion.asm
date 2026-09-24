; Byte-exact C2EFBE-C2F039 D3 clipping completion and reject epilogue.
                org     $C2EFBE
PROJECT_SEGMENT_STATUS_WORD       equ     $C45ACA
PROJECT_AND_SUBMIT_SEGMENT        equ     $C2F03A
PROJECTED_SEGMENT_SUBMIT_DIRECT   equ     $C2EFF6
CLIP_D3_FIRST                     equ     $C2F0C6
CLIP_D3_SECOND                    equ     $C2F0F4
PROJECT_SEGMENT_DEPTH_CONTINUE    equ     $C2F042

prepare_projected_segment_d3_completion:
                tst.w   d3
                bge.b   clip_nonnegative_d3
                move.w  d3,d6
                neg.w   d6
                cmp.w   d5,d6
                blt.b   projected_segment_reject
                move.w  6(a1),d2
                move.w  10(a1),d6
                neg.w   d2
                cmp.w   d2,d6
                ble.b   projected_segment_reject
                bsr.w   CLIP_D3_SECOND
                beq.b   projected_segment_submit
                tst.w   PROJECT_SEGMENT_STATUS_WORD.l
                bge.b   projected_segment_reject
                cmp.w   d5,d3
                blt.b   projected_segment_reject
                neg.w   d2
                cmp.w   d2,d6
                ble.b   projected_segment_reject
                bsr.w   CLIP_D3_FIRST
                bne.b   projected_segment_reject
                bra.b   PROJECT_AND_SUBMIT_SEGMENT
clip_nonnegative_d3:
                cmp.w   d5,d3
                blt.b   projected_segment_reject
                move.w  6(a1),d2
                move.w  10(a1),d6
                cmp.w   d2,d6
                ble.b   projected_segment_reject
                bsr.w   CLIP_D3_FIRST
                beq.b   PROJECTED_SEGMENT_SUBMIT_DIRECT
                tst.w   PROJECT_SEGMENT_STATUS_WORD.l
                bge.b   projected_segment_reject
                move.w  d3,d6
                neg.w   d6
                cmp.w   d5,d6
                blt.b   projected_segment_reject
                move.w  10(a1),d6
                neg.w   d2
                cmp.w   d2,d6
                ble.b   projected_segment_reject
                bsr.w   CLIP_D3_SECOND
                bne.b   projected_segment_reject
projected_segment_submit:
                bra.b   PROJECT_AND_SUBMIT_SEGMENT
projected_segment_retry:
                tst.w   d5
                bge.b   PROJECT_SEGMENT_DEPTH_CONTINUE
projected_segment_reject:
                unlk    a6
                moveq   #0,d0
                rts
