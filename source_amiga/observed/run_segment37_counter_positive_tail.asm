; Byte-exact static-only positive-counter tail $C30808-$C308D7.

                org     $C30808

RENDERER_LIMIT_WORD              equ $C45986
RENDERER_SIZE_WORD               equ $C45984
RENDERER_VERTICAL_OFFSET         equ $C458D8
RENDERER_POINTER_BLOCK           equ $C456B6
EXTERNAL_BLIT_HELPER             equ $C53F44
CUSTOM_BLTCON1                   equ $42
CUSTOM_BLTADAT                   equ $74
CUSTOM_BLTAFWM                   equ $44
CUSTOM_BLTALWM                   equ $46
CUSTOM_BLTAMOD                   equ $60
CUSTOM_BLTBMOD                   equ $66
SEGMENT37_STORE_FIRST            equ $C308E2
SEGMENT37_STORE_NEXT             equ $C308D8

run_segment37_counter_positive_tail:
                tst.w   RENDERER_LIMIT_WORD.l
                beq.w   segment37_positive_tail_return
                move.w  RENDERER_SIZE_WORD.l,d0
                subi.w  #$90,d0
                move.w  d0,d7
                asl.w   #3,d0
                move.w  d0,d1
                add.w   d0,d0
                add.w   d0,d0
                add.w   d1,d0
                ext.l   d0
                move.l  #$16A8,d1
                move.w  RENDERER_LIMIT_WORD.l,d6
                bgt.s   segment37_positive_tail_positive_limit
                cmpi.w  #-$14,d6
                bgt.s   segment37_positive_tail_negative_small
                move.w  #$14,d6
                bra.s   segment37_positive_tail_limit_ready
segment37_positive_tail_negative_small:
                move.w  d6,d4
                neg.w   d6
                add.w   d4,d4
                addi.w  #$28,d4
                ext.l   d4
                add.l   d4,d1
                bra.s   segment37_positive_tail_limit_ready
segment37_positive_tail_positive_limit:
                cmpi.w  #$14,d6
                blt.s   segment37_positive_tail_limit_set
                move.w  #$14,d6
segment37_positive_tail_limit_set:
segment37_positive_tail_limit_ready:
                move.w  d6,d5
                subi.w  #$14,d5
                neg.w   d5
                add.w   d5,d5
                addq.w  #1,d5
                addi.w  #$DC0,d6
                tst.w   RENDERER_VERTICAL_OFFSET.l
                ble.s   segment37_positive_tail_vertical_ready
                move.w  RENDERER_VERTICAL_OFFSET.l,d4
                lsl.w   #6,d4
                sub.w   d4,d6
segment37_positive_tail_vertical_ready:
                movea.l RENDERER_POINTER_BLOCK.l,a2
                move.l  (a2),d4
                add.l   d1,d4
                moveq   #-$1,d3
                add.l   d0,d1
                asl.w   #6,d7
                sub.w   d7,d6
                move.w  #$30A,d2
                move.l  (a2)+,d4
                add.l   d1,d4
                jsr     EXTERNAL_BLIT_HELPER.l
                move.w  #0,CUSTOM_BLTCON1(a0)
                move.w  d3,CUSTOM_BLTADAT(a0)
                move.w  d3,CUSTOM_BLTAFWM(a0)
                move.w  d3,CUSTOM_BLTALWM(a0)
                move.w  d5,CUSTOM_BLTAMOD(a0)
                move.w  d5,CUSTOM_BLTBMOD(a0)
                bsr.w   SEGMENT37_STORE_FIRST
                move.w  #$30A,d2
                bsr.w   SEGMENT37_STORE_NEXT
                move.w  #$3FA,d2
                bsr.w   SEGMENT37_STORE_NEXT
                move.w  #$3FA,d2
                bsr.w   SEGMENT37_STORE_NEXT
segment37_positive_tail_return:
                rts
