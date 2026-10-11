; Byte-exact C2F49C-C2F4DD renderer state reset and conditional line submission.
                org     $C2F49C
RENDERER_STATE_LONG             equ     $C456E6
RENDERER_STRIDE                 equ     $C45954
RENDERER_CONFIGURATION_WORD     equ     $C4566C
LINE_SUBMITTER                  equ     $C2FA7E

initialize_projected_renderer_submission:
                move.l  #$000FFFFF,RENDERER_STATE_LONG.l
                move.w  #$D,RENDERER_STRIDE.l
                tst.w   RENDERER_CONFIGURATION_WORD.l
                beq.b   projected_renderer_submission_done
                move.w  #0,d0
                move.w  #0,d1
                move.w  #8,d2
                move.w  #9,d3
                bra.b   projected_renderer_submit_line
projected_renderer_submission_alternate:
                move.w  #8,d0
                move.w  #0,d1
                move.w  #0,d2
                move.w  #9,d3
projected_renderer_submit_line:
                bsr.w   LINE_SUBMITTER
projected_renderer_submission_done:
                rts
