; Byte-exact static-only renderer-stage prelude blit $C3040C-$C30465.

                org     $C3040C

RENDERER_POINTER_BLOCK          equ $C456B6
RENDERER_BLIT_SIZE              equ $C4596E
RENDERER_BLIT_OFFSET             equ $C45968
RENDERER_BLIT_LANE_POINTER       equ $C45964
CUSTOM_BASE                      equ $DFF000
CUSTOM_DMACONR                   equ 2
CUSTOM_BLTCON0                   equ $40
CUSTOM_BLTCON1                   equ $42
CUSTOM_BLTAPTH                   equ $50
CUSTOM_BLTBPTH                   equ $4C
CUSTOM_BLTCPTH                   equ $48
CUSTOM_BLTDPTH                   equ $54
CUSTOM_BLTSIZE                   equ $58
PRELUDE_BLTCON0                  equ $0FCA

submit_renderer_stage_prelude_blit:
                movea.l RENDERER_POINTER_BLOCK.l,a2
                move.l  4(a2),d2
                move.l  (a2),d3
                move.w  RENDERER_BLIT_SIZE.l,d0
                move.l  RENDERER_BLIT_OFFSET.l,d1
                add.l   d1,d2
                add.l   d1,d3
                move.l  RENDERER_BLIT_LANE_POINTER.l,d1
                lea.l   CUSTOM_BASE,a0
                move.w  #PRELUDE_BLTCON0,d4
.wait_blitter:
                btst    #6,CUSTOM_DMACONR(a0)
                beq.s   .submit
                nop
                nop
                bra.s   .wait_blitter
.submit:
                move.w  d4,CUSTOM_BLTCON0(a0)
                move.w  #2,CUSTOM_BLTCON1(a0)
                move.l  d1,CUSTOM_BLTAPTH(a0)
                move.l  d3,CUSTOM_BLTBPTH(a0)
                move.l  d2,CUSTOM_BLTCPTH(a0)
                move.l  d2,CUSTOM_BLTDPTH(a0)
                move.w  d0,CUSTOM_BLTSIZE(a0)
                rts
