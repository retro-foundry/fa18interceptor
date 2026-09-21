; Byte-exact runtime-backed shared output phase $C2F718-$C2F765.
                org $C2F718
RENDER_OUTPUT_ENABLE equ $C456E8
RENDER_OUTPUT_MASK equ $C456EB
apply_renderer_output_mask:
 tst.w RENDER_OUTPUT_ENABLE.l
 blt.b .indirect_continue
 andi.l #$ffff,d0
 btst #0,RENDER_OUTPUT_MASK.l
 beq.b .first_done
 eor.w d4,(a3)
 moveq #-1,d0
.first_done:
 btst #1,RENDER_OUTPUT_MASK.l
 beq.b .second_done
 eor.w d5,(a2)
 moveq #-1,d0
.second_done:
 btst #2,RENDER_OUTPUT_MASK.l
 beq.b .third_done
 eor.w d6,(a1)
 moveq #-1,d0
.third_done:
 btst #3,RENDER_OUTPUT_MASK.l
 beq.b .fourth_done
 eor.w d7,(a0)
 moveq #-1,d0
.fourth_done:
 tst.l d0
 bpl.b .indirect_continue
 rts
.indirect_continue:
 jmp (a4)
