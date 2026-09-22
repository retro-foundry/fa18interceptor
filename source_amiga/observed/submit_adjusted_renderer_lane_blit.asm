; Byte-exact static-only adjusted renderer lane blit $C304FA-$C305A9.

                org     $C304FA

RENDERER_POINTER_BLOCK          equ $C456B6
RENDERER_BLIT_SIZE              equ $C4596E
RENDERER_BLIT_OFFSET             equ $C45968
RENDERER_BLIT_LANE_POINTER       equ $C45964
RENDERER_VERTICAL_INPUT          equ $C45982
RENDERER_VERTICAL_OFFSET         equ $C458D8
RENDERER_MODE_WORD               equ $C4597C
RENDERER_LIMIT_WORD              equ $C45986
CUSTOM_BASE                      equ $DFF000
CUSTOM_DMACONR                   equ 2
CUSTOM_BLTCON0                   equ $40
CUSTOM_BLTCON1                   equ $42
CUSTOM_BLTAPTH                   equ $50
CUSTOM_BLTCPTH                   equ $48
CUSTOM_BLTAMOD                   equ $60
CUSTOM_BLTBPTH                   equ $4C
CUSTOM_BLTDPTH                   equ $54
CUSTOM_BLTSIZE                   equ $58
SUBI_W_D4_OPCODE                  equ $0444
SUBI_W_D4_IMMEDIATE               equ $0003

submit_adjusted_renderer_lane_blit:
                movea.l RENDERER_POINTER_BLOCK.l,a2
                move.l  0(a2,d0.w),d2
                move.w  RENDERER_BLIT_SIZE.l,d0
                move.l  RENDERER_BLIT_OFFSET.l,d1
                add.l   d2,d1
                move.l  RENDERER_BLIT_LANE_POINTER.l,d2
                move.w  RENDERER_VERTICAL_INPUT.l,d6
                sub.w   RENDERER_VERTICAL_OFFSET.l,d6
                subi.w  #$B7,d6
                add.w   d6,d6
                add.w   d6,d6
                ext.l   d6
                move.l  #$12ADC,d5
                add.l   d6,d5
                subq.l  #2,d5
                move.w  d0,d4
                andi.w  #$3F,d4
                ; VASM shortens this to SUBQ; retain the original SUBI.W.
                dc.w    SUBI_W_D4_OPCODE,SUBI_W_D4_IMMEDIATE
                neg.w   d4
                cmpi.w  #1,d4
                beq.s   .wait_blitter
                move.w  RENDERER_MODE_WORD.l,d6
                asr.w   #4,d6
                move.w  RENDERER_LIMIT_WORD.l,d7
                addi.w  #$C,d7
                cmp.w   d7,d6
                bne.s   .wait_blitter
                subq.l  #2,d5
.wait_blitter:
                lea.l   CUSTOM_BASE,a0
.wait:
                btst    #6,CUSTOM_DMACONR(a0)
                beq.s   .select_control
                nop
                nop
                bra.s   .wait
.select_control:
                btst    #0,d3
                beq.s   .control_clear
                move.w  #$FEC,CUSTOM_BLTCON0(a0)
                bra.s   .submit
.control_clear:
                move.w  #$F4C,CUSTOM_BLTCON0(a0)
.submit:
                move.w  #2,CUSTOM_BLTCON1(a0)
                move.l  d2,CUSTOM_BLTAPTH(a0)
                move.l  d5,CUSTOM_BLTCPTH(a0)
                move.w  d4,CUSTOM_BLTAMOD(a0)
                move.l  d1,CUSTOM_BLTBPTH(a0)
                move.l  d1,CUSTOM_BLTDPTH(a0)
                move.w  d0,CUSTOM_BLTSIZE(a0)
                rts
