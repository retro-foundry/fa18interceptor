; Byte-exact observed projection-record setup $C20A52-$C20A7F.
; This branch shares the enclosing record-walker frame in A6.

                org     $C20A52

RENDERER_SELECTOR               equ     $C45954
RENDERER_STATE_WORDS            equ     $C456E6
TRANSFORMED_VERTEX_WORKSPACE    equ     $C48390

initialize_c20a52_record_projection:
                move.w  #$d,RENDERER_SELECTOR.l
                move.w  #2,d0
                move.w  #0,d1
                move.w  #2,d2
                clr.w   d3
                movem.w d0-d3,RENDERER_STATE_WORDS.l
                lea     TRANSFORMED_VERTEX_WORKSPACE.l,a3
                adda.w  (a2)+,a3
                move.w  (a2),-$38(a6)
                move.w  (a2)+,-$3a(a6)
