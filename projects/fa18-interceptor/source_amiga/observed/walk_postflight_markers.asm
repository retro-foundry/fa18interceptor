; Byte-exact C3482A-C34875 marker-orientation loops.
                org $C3482A
FILTER equ $C34876
MARKER_DONE equ $C348AC
walk_postflight_markers:
loop_a: move.b (a0)+,d0
        move.b (a0)+,d1
        blt.b enter_b
        neg.b d1
        subq.w #1,-14(a6)
        blt.b FILTER
        bra.b loop_a
enter_b: subq.w #2,a0
        subq.w #1,-4(a6)
loop_b: move.b -(a0),d1
        move.b -(a0),d0
        blt.b enter_c
        subq.w #1,-14(a6)
        blt.b FILTER
        bra.b loop_b
enter_c: addq.w #2,a0
loop_c: move.b (a0)+,d0
        move.b (a0)+,d1
        blt.b enter_d
        neg.b d0
        subq.w #1,-14(a6)
        blt.b FILTER
        bra.b loop_c
enter_d: subq.w #2,a0
        addq.w #1,-4(a6)
loop_d: move.b -(a0),d1
        move.b -(a0),d0
        blt.b MARKER_DONE
        neg.b d0
        neg.b d1
        subq.w #1,-14(a6)
        bge.b loop_d
