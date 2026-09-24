; Byte-exact C2F364-C2F389 renderer mask selection before the second mask table.
                org     $C2F364
RENDERER_SECOND_MASK_TABLE        equ     $C2F38A
CONTINUE_RENDERER_BLIT_PACKET     equ     $C2F3AC
RENDERER_BLIT_MASK_NEGATIVE       equ     $C2F3AA

prepare_projected_renderer_blit_masks:
                move.w  #$41,d7
                cmpi.w  #$F,d1
                blt.b   renderer_blit_mask_span_ready
                move.w  d1,d0
                addq.w  #1,d0
                andi.w  #$FFF0,d0
                sub.w   d0,d1
                asr.w   #4,d0
                add.w   d0,d7
renderer_blit_mask_span_ready:
                tst.w   d1
                blt.b   RENDERER_BLIT_MASK_NEGATIVE
                addq.w  #1,d7
                add.w   d1,d1
                move.w  RENDERER_SECOND_MASK_TABLE(pc,d1.w),d0
                bra.b   CONTINUE_RENDERER_BLIT_PACKET
