; Byte-exact runtime-observed postflight tuple consumer $C3129A-$C31311.

                org     $C3129A

POSTFLIGHT_RENDERER_SELECTOR     equ $C45954
POSTFLIGHT_PAIR_TABLE            equ $C3128A
POSTFLIGHT_HORIZONTAL_OFFSET     equ $C45988
POSTFLIGHT_VERTICAL_OFFSET       equ $C458D8
POSTFLIGHT_FIXED_BLITTER_ENTRY   equ $C2FA78
POSTFLIGHT_VARIANT_OTHER         equ $C31392

submit_postflight_pair_variants:
                move.w  #3,POSTFLIGHT_RENDERER_SELECTOR.l
                lea     POSTFLIGHT_PAIR_TABLE(pc),a0
                movem.w (a0)+,d0-d3
                add.w   POSTFLIGHT_HORIZONTAL_OFFSET.l,d0
                blt.s   postflight_pair_next
                cmpi.w  #$140,d0
                bge.s   postflight_pair_next
                add.w   POSTFLIGHT_HORIZONTAL_OFFSET.l,d2
                blt.s   postflight_pair_next
                cmpi.w  #$140,d2
                bge.s   postflight_pair_next
                add.w   POSTFLIGHT_VERTICAL_OFFSET.l,d1
                add.w   POSTFLIGHT_VERTICAL_OFFSET.l,d3
                move.l  a0,-(a7)
                jsr     POSTFLIGHT_FIXED_BLITTER_ENTRY.l
                movea.l (a7)+,a0
postflight_pair_next:
                movem.w (a0)+,d0-d3
                add.w   POSTFLIGHT_HORIZONTAL_OFFSET.l,d0
                blt.s   postflight_pair_finish
                cmpi.w  #$140,d0
                bge.s   postflight_pair_finish
                add.w   POSTFLIGHT_HORIZONTAL_OFFSET.l,d2
                blt.s   postflight_pair_finish
                cmpi.w  #$140,d2
                bge.s   postflight_pair_finish
                add.w   POSTFLIGHT_VERTICAL_OFFSET.l,d1
                add.w   POSTFLIGHT_VERTICAL_OFFSET.l,d3
                jsr     POSTFLIGHT_FIXED_BLITTER_ENTRY.l
postflight_pair_finish:
                bra.w   POSTFLIGHT_VARIANT_OTHER
