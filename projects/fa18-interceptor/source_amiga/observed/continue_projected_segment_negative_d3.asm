; Byte-exact continuation $C2EEB4-$C2EEC9 after the negative-D3 clip path.
; Retains the unresolved C45ACA branch and transfers to the shared clip helper.
                org     $C2EEB4
PROJECT_SEGMENT_STATUS_WORD       equ     $C45ACA
CONTINUE_PROJECTED_SEGMENT_CLIP   equ     $C2EECA
CLIP_FIRST_PROJECTED_SEGMENT      equ     $C2F0C6
PROJECTED_SEGMENT_REJECT_PATH     equ     $C2EFF6
continue_projected_segment_negative_d3:
                tst.w   PROJECT_SEGMENT_STATUS_WORD.l
                bge.b   CONTINUE_PROJECTED_SEGMENT_CLIP
                neg.w   d2
                cmp.w   d2,d6
                ble.b   CONTINUE_PROJECTED_SEGMENT_CLIP
                bsr.w   CLIP_FIRST_PROJECTED_SEGMENT
                beq.w   PROJECTED_SEGMENT_REJECT_PATH
