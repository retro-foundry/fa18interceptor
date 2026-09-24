; Byte-exact C2EE68-C2EE93 bridge in the C2EE4A segment-preparation helper.
; Handles the fall-through after the first triple's D3/D5 test. It compares
; the second triple depth pair, invokes established clipping helpers, and
; transfers either to the raw C2EECA continuation or C2F03A projection tail.
; The live meanings of C45ACA and the external continuations remain unresolved.
                org     $C2EE68
PROJECT_SEGMENT_STATUS_WORD      equ     $C45ACA
REJECT_PROJECTED_SEGMENT         equ     $C2EE44
CLIP_FIRST_PROJECTED_SEGMENT     equ     $C2F0C6
CLIP_SECOND_PROJECTED_SEGMENT    equ     $C2F0F4
PROJECTED_SEGMENT_REJECT_PATH    equ     $C2EFF6
PROJECTED_SEGMENT_CONTINUATION   equ     $C2EECA
PROJECT_AND_SUBMIT_SEGMENT       equ     $C2F03A
prepare_projected_segment_second_depth_bridge:
                move.w  6(a1),d2
                move.w  10(a1),d6
                cmp.w   d2,d6
                ble.b   REJECT_PROJECTED_SEGMENT
                bsr.w   CLIP_FIRST_PROJECTED_SEGMENT
                beq.w   PROJECTED_SEGMENT_REJECT_PATH
                tst.w   PROJECT_SEGMENT_STATUS_WORD.l
                bge.b   PROJECTED_SEGMENT_CONTINUATION
                neg.w   d2
                cmp.w   d2,d6
                ble.b   PROJECTED_SEGMENT_CONTINUATION
                bsr.w   CLIP_SECOND_PROJECTED_SEGMENT
                bne.b   PROJECTED_SEGMENT_CONTINUATION
                bra.w   PROJECT_AND_SUBMIT_SEGMENT
