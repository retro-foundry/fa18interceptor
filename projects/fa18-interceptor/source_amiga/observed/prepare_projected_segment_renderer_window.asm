; Byte-exact C2F250-C2F2AF renderer window/index preparation.
                org     $C2F250
RENDERER_LAYOUT_BASE              equ     $C4FE1C
RENDERER_WINDOW_LIMIT             equ     $C45984
PROJECTED_RENDERER_EXIT           equ     $C2F47E

prepare_projected_segment_renderer_window:
                move.w  d0,-6(a6)
                movea.l RENDERER_LAYOUT_BASE.l,a4
                move.w  (a4)+,d4
                move.w  d4,d6
                subq.w  #1,d4
                sub.w   d4,d1
                cmpi.w  #1,d1
                bge.b   renderer_window_normalize
                subq.w  #1,d1
                add.w   d1,d4
                bge.b   renderer_window_negative_index
                add.w   d4,d6
                ble.w   PROJECTED_RENDERER_EXIT
renderer_window_negative_index:
                neg.w   d1
                asl.w   #2,d1
                adda.w  d1,a4
                moveq   #1,d1
                bra.b   renderer_window_normalize
renderer_window_advance:
                move.w  d1,d5
                sub.w   RENDERER_WINDOW_LIMIT.l,d5
                sub.w   d6,d1
                sub.w   d4,d1
                cmp.w   RENDERER_WINDOW_LIMIT.l,d1
                bgt.w   PROJECTED_RENDERER_EXIT
                sub.w   d5,d6
                bge.b   renderer_window_bounds
                add.w   d6,d4
                bra.b   renderer_window_bounds
renderer_window_normalize:
                add.w   d4,d1
                add.w   d6,d1
                cmp.w   RENDERER_WINDOW_LIMIT.l,d1
                bge.b   renderer_window_advance
                sub.w   d6,d1
                sub.w   d4,d1
renderer_window_bounds:
                move.w  d1,d5
                asl.w   #3,d5
