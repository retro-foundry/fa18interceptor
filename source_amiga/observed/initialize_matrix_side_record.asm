; Byte-exact matrix-side record setup $C1342C-$C13487.
; The bounded run003 frame-6000 route takes the final BEQ to $C13490.

                org     $C1342C

SELECTED_RECORD_INDEX            equ $C459B4
RECORD_ARRAY_BASE                equ $C46184
MATRIX_SIDE_RECORD               equ $C18210

initialize_matrix_side_record:
                link.w  a6,#-$20
                movem.l d2-d3/a2-a3,-(a7)
                move.w  SELECTED_RECORD_INDEX.l,d0
                moveq   #9,d1
                move.w  d0,-$02(a6)
                ext.l   d0
                asl.l   d1,d0
                movea.l d0,a0
                adda.l  #RECORD_ARRAY_BASE,a0
                move.l  a0,MATRIX_SIDE_RECORD.l
                addq.l  #2,a0
                movea.l MATRIX_SIDE_RECORD.l,a1
                dc.w    $D2FC,$0056              ; adda.w #$56,a1
                movea.l MATRIX_SIDE_RECORD.l,a2
                dc.w    $D4FC,$0058              ; adda.w #$58,a2
                movea.l MATRIX_SIDE_RECORD.l,a3
                dc.w    $D6FC,$005A              ; adda.w #$5A,a3
                move.l  a0,-$0A(a6)
                move.l  a1,-$0E(a6)
                move.l  a2,-$12(a6)
                move.l  a3,-$16(a6)
                tst.w   -$02(a6)
                beq.b   $C13490
