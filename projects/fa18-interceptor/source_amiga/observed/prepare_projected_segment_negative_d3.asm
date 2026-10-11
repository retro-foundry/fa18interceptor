; Byte-exact observed negative-D3 clipping continuation $C2EE94-$C2EEB3.

                org     $C2EE94

CONTINUE_NEGATIVE_D4_CLIP       equ     $C2EF58
PROJECT_SEGMENT_INTERSECTION    equ     $C2F0F4
RETURN_REJECTED_SEGMENT         equ     $C2F02E

prepare_projected_segment_negative_d3:
                move.w  d3,d6
                neg.w   d6
                cmp.w   d5,d6
                blt.w   CONTINUE_NEGATIVE_D4_CLIP
                move.w  6(a1),d2
                move.w  10(a1),d6
                neg.w   d2
                cmp.w   d2,d6
                ble.b   $C2EE44
                bsr.w   PROJECT_SEGMENT_INTERSECTION
                beq.w   RETURN_REJECTED_SEGMENT
