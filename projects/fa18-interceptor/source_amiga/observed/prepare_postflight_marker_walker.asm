; Byte-exact C347F2-C34829 frame and window setup for marker walker.
                org     $C347F2
RENDERER_X_OFFSET equ $C45988
prepare_postflight_marker_walker:
                link.w a6,#-16
                move.w d0,-2(a6)
                move.w d1,-4(a6)
                move.w d2,-14(a6)
                move.w #$55,d0
                add.w RENDERER_X_OFFSET.l,d0
                move.w d0,-6(a6)
                move.w #$E9,d0
                add.w RENDERER_X_OFFSET.l,d0
                move.w d0,-8(a6)
                move.w #$2D,-10(a6)
                move.w #$90,-12(a6)
