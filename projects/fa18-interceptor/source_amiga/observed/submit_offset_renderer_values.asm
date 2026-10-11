; Byte-exact $C2F63A-$C2F66D offset-and-restore renderer wrapper.
                org $C2F63A
RENDER_X_OFFSET equ $C45988
RENDER_Y_OFFSET equ $C458D8
RENDER_POINTERS equ $C456B6
submit_offset_renderer_values:
 add.w RENDER_X_OFFSET.l,d0
 blt.b $C2F622
 cmpi.w #$13f,d0
 bge.b $C2F622
 add.w RENDER_Y_OFFSET.l,d1
 move.w d0,-(sp)
 move.w d1,-(sp)
 movea.l RENDER_POINTERS.l,a1
 lea $C2F7C6.l,a3
 lea $C2F7E6.l,a4
 bsr.w $C2F688
 move.w (sp)+,d1
 move.w (sp)+,d0
 rts
