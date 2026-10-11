; Byte-exact shared projection tail $C2F03A-$C2F0C5.
; Called by the clipped segment-preparation path with A6 frame, A1 pair, A0
; output pair, and C45AC6 holding signed x/y/depth words.

PROJECTED_COMPONENTS          equ     $C45AC6
PROJECT_ERROR_CODE             equ     $C4599E
LINE_SUBMITTER                 equ     $C2FA7E
CLIP_RETRY                     equ     $C2EE44
CLIP_CONTINUE                  equ     $C2EE60

                org     $C2F03A

project_clamp_and_submit_segment_pair:
                movem.w PROJECTED_COMPONENTS.l,d3-d5
                tst.w   d5
                ble.s   project_segment_reject_depth
                muls.w  #$A0,d3
                divs.w  d5,d3
                addi.w  #$A0,d3
                blt.s   project_segment_clamp_x_low
                cmpi.w  #$140,d3
                bge.s   project_segment_clamp_x_high
project_segment_y:
                muls.w  #$5A,d4
                divs.w  d5,d4
                addi.w  #$5A,d4
                blt.s   project_segment_clamp_y_low
                cmpi.w  #$B4,d4
                bge.s   project_segment_clamp_y_high
project_segment_store:
                subi.w  #$13F,d3
                neg.w   d3
                subi.w  #$B3,d4
                neg.w   d4
                move.w  d3,(a0)+
                move.w  d4,(a0)+
                subq.w  #1,-$2(a6)
                beq.s   project_segment_continue_clip
                movem.w $C4B390.l,d0-d3
                jsr     LINE_SUBMITTER.l
                unlk    a6
                moveq   #1,d0
                rts
project_segment_reject_depth:
                move.w  #$16,PROJECT_ERROR_CODE.l
                bra.w   CLIP_RETRY
project_segment_continue_clip:
                movem.w (a1),d0-d5
                movem.w d3-d5,(a1)
                movem.w d0-d2,$6(a1)
                bra.w   CLIP_CONTINUE
project_segment_clamp_x_low:
                clr.w   d3
                bra.s   project_segment_y
project_segment_clamp_y_low:
                clr.w   d4
                bra.s   project_segment_store
project_segment_clamp_x_high:
                move.w  #$13F,d3
                bra.s   project_segment_y
project_segment_clamp_y_high:
                move.w  #$B3,d4
                bra.s   project_segment_store
