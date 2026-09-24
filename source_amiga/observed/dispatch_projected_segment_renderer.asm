; Byte-exact C2F1B8-C2F1C5 renderer dispatch and bounded entry precondition.
                org     $C2F1B8
SUBMIT_ADJACENT_RENDERER_VALUES    equ     $C2F60A

dispatch_projected_segment_renderer:
                jsr     SUBMIT_ADJACENT_RENDERER_VALUES.l
                rts

enter_projected_segment_renderer:
                move.w  d6,d5
                subq.w  #1,d5
                blt.b   dispatch_projected_segment_renderer
