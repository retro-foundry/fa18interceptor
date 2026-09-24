; Byte-exact C2F2B0-C2F341 renderer blit-window setup before its mask table.
                org     $C2F2B0
ACTIVE_PLANE_POINTERS             equ     $C456B6
RENDERER_STRIDE                   equ     $C45954
CUSTOM_CHIP_BASE                  equ     $DFF000
CONTINUE_RENDERER_BLIT_WINDOW     equ     $C2F364
RENDERER_MASK_TABLE               equ     $C2F342

prepare_projected_renderer_blit_window:
                move.w  d5,d1
                add.w   d1,d1
                add.w   d1,d1
                add.w   d5,d1
                movea.l ACTIVE_PLANE_POINTERS.l,a1
                movea.l 12(a1),a1
                adda.w  d1,a1
                lea     CUSTOM_CHIP_BASE.l,a2
renderer_wait_blitter:
                btst.b  #6,2(a2)
                beq.b   renderer_blitter_ready
                nop
                nop
                bra.b   renderer_wait_blitter
renderer_blitter_ready:
                move.w  #0,$42(a2)
                move.w  #$FFFF,$74(a2)
                move.w  RENDERER_STRIDE.l,-2(a6)
                move.w  -2(a6),-4(a6)
                move.w  (a4),d0
                add.w   -6(a6),d0
                bge.b   renderer_blit_left_ok
                clr.w   d0
renderer_blit_left_ok:
                move.w  2(a4),d7
                add.w   -6(a6),d7
                cmpi.w  #$140,d7
                blt.b   renderer_blit_right_ok
                move.w  #$13F,d7
renderer_blit_right_ok:
                sub.w   d0,d7
                move.w  d0,d1
                andi.w  #$F,d0
                andi.w  #$FFF0,d1
                asr.w   #3,d1
                lea     (a1,d1.w),a0
                move.w  d7,d1
                move.w  d0,d7
                neg.w   d0
                addi.w  #$10,d0
                cmp.w   d1,d0
                ble.b   renderer_blit_span_ok
                move.w  d1,d0
                addq.w  #1,d0
renderer_blit_span_ok:
                sub.w   d0,d1
                add.w   d0,d0
                move.w  RENDERER_MASK_TABLE(pc,d0.w),d0
                ror.w   d7,d0
                move.w  d0,$44(a2)
                bra.b   CONTINUE_RENDERER_BLIT_WINDOW
