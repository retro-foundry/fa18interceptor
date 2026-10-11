; Byte-exact sibling projection setup $C20C38-$C20C65.

                org     $C20C38

RENDERER_SELECTOR               equ     $C45954
RENDERER_STATE_WORDS            equ     $C456E6
TRANSFORMED_VERTEX_WORKSPACE    equ     $C48390

initialize_c20c38_record_projection:
                movem.l a1/a5,-(sp)
                move.w  #$d,RENDERER_SELECTOR.l
                move.w  #2,d0
                move.w  #0,d1
                move.w  #2,d2
                clr.w   d3
                movem.w d0-d3,RENDERER_STATE_WORDS.l
                lea     TRANSFORMED_VERTEX_WORKSPACE.l,a3
                adda.w  (a2)+,a3
                move.w  (a2)+,-$38(a6)
