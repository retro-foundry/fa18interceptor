; Byte-exact C2F1C6-C2F24F setup and layout construction for the renderer entry.
                org     $C2F1C6
RENDERER_LAYOUT_BASE              equ     $C4FE1C
CONTINUE_PROJECTED_RENDERER       equ     $C2F250

prepare_projected_segment_renderer_layout:
                link.w  a6,#-8
                movea.l RENDERER_LAYOUT_BASE.l,a2
                subq.w  #1,d5
                bge.b   renderer_layout_nonempty
                move.w  d6,(a2)+
                move.l  #$FFFF0001,(a2)
                bra.w   CONTINUE_PROJECTED_RENDERER
renderer_layout_nonempty:
                cmpi.w  #$7F,d6
                ble.b   renderer_layout_store_count
                move.w  #$7F,d6
renderer_layout_store_count:
                move.w  d6,(a2)+
                move.w  d6,d2
                move.w  d2,d5
                add.w   d5,d5
                add.w   d5,d5
                lea     (a2,d5.w),a3
                clr.w   d3
                moveq   #3,d4
                sub.w   d2,d4
                sub.w   d2,d4
renderer_layout_loop:
                cmp.w   d2,d3
                bge.b   renderer_layout_finish
renderer_layout_body:
                neg.w   d3
                move.w  d3,(a2)
                neg.w   d3
                move.w  d3,2(a2)
                neg.w   d2
                move.w  d2,(a3)
                neg.w   d2
                move.w  d2,2(a3)
                tst.w   d4
                bge.b   renderer_layout_stride
                add.w   d3,d4
                add.w   d3,d4
                add.w   d3,d4
                add.w   d3,d4
                addq.w  #6,d4
                bra.b   renderer_layout_advance
renderer_layout_stride:
                move.w  d3,d5
                sub.w   d2,d5
                add.w   d5,d5
                add.w   d5,d5
                addi.w  #10,d5
                add.w   d5,d4
                subq.w  #1,d2
                addq.w  #4,a2
renderer_layout_advance:
                subq.w  #4,a3
                addq.w  #1,d3
                cmp.w   d2,d3
                blt.b   renderer_layout_body
renderer_layout_finish:
                cmp.w   d2,d3
                bne.b   CONTINUE_PROJECTED_RENDERER
                neg.w   d3
                move.w  d3,(a2)
                neg.w   d3
                move.w  d3,2(a2)
