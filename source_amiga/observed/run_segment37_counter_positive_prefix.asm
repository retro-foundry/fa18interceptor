; Byte-exact static-only positive-counter prefix $C3076C-$C30807.

                org     $C3076C

SEGMENT37_COUNTER                equ $C45836
RENDERER_POINTER_BLOCK           equ $C456B6
SEGMENT37_POINTER_TABLE          equ $C30752
RENDERER_VERTICAL_OFFSET         equ $C458D8
RENDERER_WORK_OFFSET             equ $C45918
EXTERNAL_HELPER                  equ $C310E2
EXTERNAL_BLIT_HELPER             equ $C53F44
CUSTOM_BASE                      equ $DFF000
CUSTOM_BLTCON1                   equ $42
CUSTOM_BLTAFWM                   equ $44
CUSTOM_BLTALWM                   equ $46
CUSTOM_BLTAMOD                   equ $64
CUSTOM_BLTBMOD                   equ $66
SEGMENT37_STORE_FIRST            equ $C30904
SEGMENT37_STORE_NEXT             equ $C308F4
SEGMENT37_POST_PREFIX            equ $C30808

run_segment37_counter_positive_prefix:
                subq.b  #1,SEGMENT37_COUNTER.l
                lea.l   CUSTOM_BASE,a0
                movea.l RENDERER_POINTER_BLOCK.l,a2
                lea.l   SEGMENT37_POINTER_TABLE(pc),a1
                move.w  #$9F0,d2
                move.l  #$16A8,d1
                movea.w #$14,a4
                move.w  #$37,d6
                tst.w   RENDERER_VERTICAL_OFFSET.l
                ble.s   .vertical_ready
                sub.w   RENDERER_VERTICAL_OFFSET.l,d6
.vertical_ready:
                asl.w   #6,d6
                addi.w  #$14,d6
                movea.w #1,a5
                move.w  #0,d7
                add.l   RENDERER_WORK_OFFSET.l,d1
                bsr.w   EXTERNAL_HELPER
                blt.w   SEGMENT37_POST_PREFIX
                add.w   d7,d5
                add.w   d7,d7
                ext.l   d7
                move.l  (a2)+,d4
                add.l   d1,d4
                sub.w   d5,d6
                add.w   d5,d5
                movea.l (a1)+,a3
                move.l  (a3),d0
                add.l   d7,d0
                addq.w  #1,d5
                jsr     EXTERNAL_BLIT_HELPER.l
                move.w  #0,CUSTOM_BLTCON1(a0)
                move.w  #$FFFF,CUSTOM_BLTAFWM(a0)
                move.w  #$FFFF,CUSTOM_BLTALWM(a0)
                move.w  d5,CUSTOM_BLTAMOD(a0)
                subq.w  #1,d5
                add.w   a5,d5
                move.w  d5,CUSTOM_BLTBMOD(a0)
                bsr.w   SEGMENT37_STORE_FIRST
                bsr.w   SEGMENT37_STORE_NEXT
                bsr.w   SEGMENT37_STORE_NEXT
                bsr.w   SEGMENT37_STORE_NEXT
