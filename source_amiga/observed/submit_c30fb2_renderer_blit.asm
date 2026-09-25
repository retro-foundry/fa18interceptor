; Byte-exact runtime-observed renderer blit submission $C30FB2-$C310A9.

                org     $C30FB2

RENDERER_POINTER_BLOCK          equ     $C456B6
RENDERER_WORK_OFFSET             equ     $C45918
RENDERER_OUTPUT_STATE            equ     $C459A2
RENDERER_CLIP_HELPER             equ     $C310E2
EXTERNAL_BLIT_HELPER             equ     $C53F44
FIXED_BLITTER_LINE               equ     $C2FA78
CUSTOM_BASE                      equ     $DFF000
CUSTOM_BLTCON0                   equ     $40
CUSTOM_BLTCON1                   equ     $42
CUSTOM_BLTAFWM                   equ     $44
CUSTOM_BLTALWM                   equ     $46
CUSTOM_BLTCPTH                   equ     $48
CUSTOM_BLTBPTH                   equ     $4C
CUSTOM_BLTAMOD                   equ     $60
CUSTOM_BLTBMOD                   equ     $62
CUSTOM_BLTDMOD                   equ     $66
CUSTOM_BLTAPTH                   equ     $54
CUSTOM_BLTSIZE                   equ     $58
RENDERER_LIMIT                   equ     $C456E6
RENDERER_Y_OFFSET                equ     $C45988
RENDERER_LINE_ACTIVE             equ     $C45954

submit_c30fb2_renderer_blit:
                move.w  d0,RENDERER_OUTPUT_STATE.l
                ori.w   #$8000,RENDERER_OUTPUT_STATE.l
                lea     CUSTOM_BASE.l,a0
                movea.l RENDERER_POINTER_BLOCK.l,a2
                move.l  #$17B0,d1
                movea.w #2,a4
                movea.w #$23,a5
                move.w  #$1C3,d3
                move.w  #$C,d7
                add.l   RENDERER_WORK_OFFSET.l,d1
                bsr.w   RENDERER_CLIP_HELPER
                blt.b   $C30F76
                add.w   d7,d5
                add.l   12(a2),d1
                sub.w   d5,d3
                add.w   d5,d5
                movea.w d5,a4
                move.l  #$FFFF0000,d6
                asr.w   #1,d0
                move.w  d0,d4
                andi.w  #$F,d0
                beq.b   .bitplane_ready
                subq.l  #2,d1
                swap    d6
                neg.w   d0
                addi.w  #$10,d0
                ror.w   #4,d0
.bitplane_ready:
                andi.w  #$F0,d4
                asr.w   #3,d4
                ext.l   d4
                addi.l  #$12AFC,d4
                add.w   d7,d7
                ext.l   d7
                add.l   d7,d4
                move.w  #$73A,d2
                move.w  a4,d5
                ; Keep ADDI encoding: VASM otherwise shortens this to ADDQ.
                dc.w    $0645,$0007
                jsr     EXTERNAL_BLIT_HELPER.l
                move.w  d2,CUSTOM_BLTCON0(a0)
                move.w  d0,CUSTOM_BLTCON1(a0)
                move.w  #-$1,$74(a0)
                move.w  d6,CUSTOM_BLTALWM(a0)
                swap    d6
                move.w  d6,CUSTOM_BLTAFWM(a0)
                move.w  d5,CUSTOM_BLTBMOD(a0)
                ; Keep SUBI encoding: VASM otherwise shortens this to SUBQ.
                dc.w    $0445,$0007
                add.w   a5,d5
                move.w  d5,CUSTOM_BLTAMOD(a0)
                move.w  d5,CUSTOM_BLTDMOD(a0)
                move.l  d4,CUSTOM_BLTBPTH(a0)
                move.l  d1,CUSTOM_BLTCPTH(a0)
                move.l  d1,CUSTOM_BLTAPTH(a0)
                move.w  d3,CUSTOM_BLTSIZE(a0)
                move.l  #$FFFFF,RENDERER_LIMIT.l
                move.w  #$D0,d0
                add.w   RENDERER_Y_OFFSET.l,d0
                blt.b   .done
                cmpi.w  #$140,d0
                bge.b   .done
                move.w  #$96,d1
                move.w  d0,d2
                move.w  d1,d3
                addq.w  #7,d3
                move.w  #1,RENDERER_LINE_ACTIVE.l
                jsr     FIXED_BLITTER_LINE.l
.done:
                rts
