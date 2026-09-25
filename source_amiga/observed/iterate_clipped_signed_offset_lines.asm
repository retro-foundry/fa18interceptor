; Byte-exact observed signed three-byte line-list iterator $C2BAA8-$C2BAEF.
; Each record is signed dy, signed dx, unsigned width-minus-one; $80 ends list.
; The submitted segment must fit the 320x180 logical renderer bounds.

                org     $C2BAA8

LINE_LIST_END                   equ     $80
RENDER_MAX_Y                    equ     $00B3
RENDER_MAX_X                    equ     $013F
LINE_EMITTER                    equ     $C2FA7E

iterate_clipped_signed_offset_lines:
                move.b  (a1)+,d5
                cmpi.b  #LINE_LIST_END,d5
                beq.s   .return
                move.b  (a1)+,d6
                move.b  (a1)+,d7
                subq.b  #1,d7
                ext.w   d5
                ext.w   d6
                ext.w   d7
                move.w  d0,-(a7)
                move.w  d1,-(a7)
                move.w  d0,d2
                move.w  d1,d3
                add.w   d5,d1
                blt.s   .restore
                cmpi.w  #RENDER_MAX_Y,d1
                bgt.s   .restore
                add.w   d5,d3
                add.w   d6,d0
                blt.s   .restore
                add.w   d6,d2
                add.w   d7,d2
                cmpi.w  #RENDER_MAX_X,d2
                bgt.s   .restore
                move.l  a1,-(a7)
                jsr     LINE_EMITTER.l
                movea.l (a7)+,a1
.restore:
                move.w  (a7)+,d1
                move.w  (a7)+,d0
                bra.s   iterate_clipped_signed_offset_lines
.return:
                rts
