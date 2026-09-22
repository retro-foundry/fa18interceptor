; Byte-exact runtime-backed renderer lane-blit prefix $C30466-$C304B1.

                org     $C30466

RENDERER_LINE_CONTROL           equ $C45956
RENDERER_POINTER_BLOCK          equ $C456B6
RENDERER_BLIT_SIZE              equ $C4596E
RENDERER_BLIT_OFFSET             equ $C45968
RENDERER_BLIT_LANE_POINTER       equ $C45964
CUSTOM_BASE                      equ $DFF000
CUSTOM_DMACONR                   equ 2
CUSTOM_BLTCON0                   equ $40
LANE_BLTCON0_A                   equ $0DFC
LANE_BLTCON0_CONTINUE            equ $C304E2
LANE_BLTCON0_B                   equ $C304D4
LANE_BLTCON0_C                   equ $C304DC

submit_renderer_lane_blit_prefix:
                lsr.w   RENDERER_LINE_CONTROL.l
                movea.l RENDERER_POINTER_BLOCK.l,a2
                move.l  0(a2,d0.w),d2
                move.w  RENDERER_BLIT_SIZE.l,d0
                move.l  RENDERER_BLIT_OFFSET.l,d1
                add.l   d2,d1
                move.l  RENDERER_BLIT_LANE_POINTER.l,d2
                lea.l   CUSTOM_BASE,a0
.wait_blitter:
                btst    #6,CUSTOM_DMACONR(a0)
                beq.s   .select_control
                nop
                nop
                bra.s   .wait_blitter
.select_control:
                btst    #0,d4
                bne.s   LANE_BLTCON0_C
                btst    #0,d3
                beq.s   LANE_BLTCON0_B
                move.w  #LANE_BLTCON0_A,CUSTOM_BLTCON0(a0)
                bra.s   LANE_BLTCON0_CONTINUE
