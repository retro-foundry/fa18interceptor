; Byte-exact partially runtime-backed segment-37 table renderer $C30EAA-$C30F73.

                org     $C30EAA

RENDERER_POINTER_BLOCK           equ $C456B6
RENDERER_LONG_OFFSET              equ $C45918
EXTERNAL_BLIT_HELPER              equ $C53F44
RENDERER_CLIP_HELPER              equ $C310E2
CUSTOM_BASE                       equ $DFF000
CUSTOM_BLTCON1                    equ $42
CUSTOM_BLTAFWM                    equ $44
CUSTOM_BLTALWM                    equ $46
CUSTOM_BLTCPTH                    equ $48
CUSTOM_BLTBPTH                    equ $4C
CUSTOM_BLTAPTH                    equ $50
CUSTOM_BLTDPTH                    equ $54
CUSTOM_BLTSIZE                    equ $58
CUSTOM_BLTCMOD                    equ $60
CUSTOM_BLTBMOD                    equ $62
CUSTOM_BLTAMOD                    equ $64
CUSTOM_BLTDMOD                    equ $66

run_segment37_table_renderer:
                ; MOVE.L #0,A0. VASM otherwise substitutes SUBA.L A0,A0.
                dc.w    $207C
                dc.l    0
                movea.l RENDERER_POINTER_BLOCK.l,a2
                add.l   RENDERER_LONG_OFFSET.l,d1
                bsr.w   RENDERER_CLIP_HELPER
                blt.w   segment37_table_renderer_reject
                add.w   d7,d5
                add.w   d7,d7
                ext.l   d7
                sub.w   d5,d6
                add.w   d5,d5
                addq.w  #1,d5
                swap    d2
                move.w  a0,d2
                add.w   d2,d5
                swap    d2
                ext.l   d5
segment37_table_renderer_wrap_offset:
                cmpi.l  #$28,d1
                bge.s   segment37_table_renderer_submit
                addi.l  #$28,d1
                add.l   a4,d7
                add.l   a4,d7
                subi.w  #$40,d6
                bge.s   segment37_table_renderer_wrap_offset
                bra.w   segment37_table_renderer_reject
segment37_table_renderer_submit:
                move.l  (a2)+,d4
                add.l   d1,d4
                movea.l (a1)+,a3
                move.l  (a3),d3
                add.l   d7,d3
                add.l   d7,d0
                lea     CUSTOM_BASE.l,a0
                jsr     EXTERNAL_BLIT_HELPER.l
                move.w  #0,CUSTOM_BLTCON1(a0)
                move.w  #-$1,CUSTOM_BLTAFWM(a0)
                move.w  #-$1,CUSTOM_BLTALWM(a0)
                move.w  d5,CUSTOM_BLTBMOD(a0)
                swap    d2
                sub.w   d2,d5
                swap    d2
                move.w  d5,CUSTOM_BLTAMOD(a0)
                subq.w  #1,d5
                add.w   a5,d5
                move.w  d5,CUSTOM_BLTCMOD(a0)
                move.w  d5,CUSTOM_BLTDMOD(a0)
                bsr.w   segment37_table_renderer_store
                bsr.w   segment37_table_renderer_next
                bsr.w   segment37_table_renderer_next

segment37_table_renderer_next:
                move.l  (a2)+,d4
                add.l   d1,d4
                movea.l (a1)+,a3
                move.l  (a3),d3
                add.l   d7,d3
                jsr     EXTERNAL_BLIT_HELPER.l
segment37_table_renderer_store:
                move.w  d2,$40(a0)
                move.l  d0,CUSTOM_BLTAPTH(a0)
                move.l  d3,CUSTOM_BLTBPTH(a0)
                move.l  d4,CUSTOM_BLTCPTH(a0)
                move.l  d4,CUSTOM_BLTDPTH(a0)
                move.w  d6,CUSTOM_BLTSIZE(a0)
                rts

segment37_table_renderer_reject:
                moveq   #-$1,d0
                rts
