; Byte-exact geometry-stage setup $C20904-$C20953.
; Record and output ownership remain structural.

                org     $C20904

GEOMETRY_STATE_BLOCK            equ $C456E6
SELECTED_RECORD_WORD            equ $C45954
OFFSET_VERTEX_TABLE             equ $C48390

initialize_c20904_geometry_stage:
                move.w  #$F,d0
                move.w  #-$1,d1
                clr.w   d2
                clr.w   d3
                movem.w d0-d3,GEOMETRY_STATE_BLOCK.l
                move.w  (a2)+,SELECTED_RECORD_WORD.l
                clr.w   -126(a6)
                lea     OFFSET_VERTEX_TABLE.l,a3
                adda.w  (a2)+,a3
                move.w  (a2),-56(a6)
                move.w  (a2)+,-58(a6)
                movem.l a5/a2/a1,-(sp)
                movem.w (a3)+,d1-d6
                sub.w   (a3),d1
                sub.w   2(a3),d2
                sub.w   4(a3),d3
                sub.w   (a3),d4
                sub.w   2(a3),d5
                sub.w   4(a3),d6
                movem.w d1-d6,-70(a6)
