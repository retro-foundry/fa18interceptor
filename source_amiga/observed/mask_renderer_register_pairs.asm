; Byte-exact runtime-backed mask phase $C2F6D8-$C2F717 of shared renderer.
                org $C2F6D8
RENDER_PAIR_MASK equ $C456E7
mask_renderer_register_pairs:
 btst #0,RENDER_PAIR_MASK.l
 bne.b .first_enabled
 move.w #-1,d0
 clr.w d4
.first_enabled:
 btst #1,RENDER_PAIR_MASK.l
 bne.b .second_enabled
 move.w #-1,d1
 clr.w d5
.second_enabled:
 btst #2,RENDER_PAIR_MASK.l
 bne.b .third_enabled
 move.w #-1,d2
 clr.w d6
.third_enabled:
 btst #3,RENDER_PAIR_MASK.l
 bne.b .complete
 move.w #-1,d3
 clr.w d7
.complete:
