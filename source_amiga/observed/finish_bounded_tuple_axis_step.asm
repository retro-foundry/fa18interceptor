; Byte-exact observed bounded-tuple axis-step tail $C302C4-$C302E3.
; It advances one axis, selects the first/second renderer helper from the
; remaining signed distance, then begins the adjacent absolute-distance path.

                org     $C302C4

RENDERER_FIRST_AXIS_HELPER       equ     $C2F5F4
RENDERER_SECOND_AXIS_HELPER      equ     $C2F60A

finish_bounded_tuple_axis_step:
                addq.w  #1,d1
                cmp.w   a4,d1
                bgt.b   $C302DA
                subq.w  #1,d6
                bge.b   .use_second_helper
                bsr.w   RENDERER_FIRST_AXIS_HELPER
                bra.b   $C302DA
.use_second_helper:
                move.w  d2,d0
                bsr.w   RENDERER_SECOND_AXIS_HELPER
                moveq   #1,d0
                rts
                move.w  d2,d6
                sub.w   d0,d6
                bge.b   $C302E6
