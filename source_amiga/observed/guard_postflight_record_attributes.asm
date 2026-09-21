; Byte-exact static-only postflight record-attribute guards $C3149C-$C31517.

                org     $C3149C

POSTFLIGHT_MODE_BYTE             equ $C458A6
POSTFLIGHT_CURRENT_INDEX         equ $C459C0
POSTFLIGHT_RECORD_OFFSET          equ $C458DE
POSTFLIGHT_DELAY_BYTE            equ $C4586F
POSTFLIGHT_ATTRIBUTE_SKIP        equ $C315B0
POSTFLIGHT_ATTRIBUTE_CLEAR       equ $C315AA

guard_postflight_record_attributes:
                move.b  POSTFLIGHT_MODE_BYTE.l,d1
                cmpi.b  #$7D,d1
                beq.w   POSTFLIGHT_ATTRIBUTE_SKIP
                subq.b  #2,d1
                beq.w   POSTFLIGHT_ATTRIBUTE_SKIP
                move.l  d5,d1
                bge.s   postflight_attribute_d5_positive
                neg.l   d1
postflight_attribute_d5_positive:
                cmpi.l  #$10000,d1
                bgt.w   POSTFLIGHT_ATTRIBUTE_CLEAR
                move.l  a5,d1
                bge.s   postflight_attribute_a5_positive
                neg.l   d1
postflight_attribute_a5_positive:
                cmpi.l  #$10000,d1
                bgt.w   POSTFLIGHT_ATTRIBUTE_CLEAR
                bset    #6,$20(a1)
                move.b  $62(a1),d1
                andi.b  #$F0,d1
                cmpi.b  #$20,d1
                beq.w   POSTFLIGHT_ATTRIBUTE_SKIP
                tst.w   POSTFLIGHT_CURRENT_INDEX.l
                blt.s   postflight_attribute_done
                cmpi.b  #$10,d1
                bne.s   postflight_attribute_done
                tst.w   POSTFLIGHT_RECORD_OFFSET.l
                bne.s   postflight_attribute_done
                tst.b   POSTFLIGHT_DELAY_BYTE.l
                beq.s   postflight_attribute_set_delay
                blt.s   postflight_attribute_done
postflight_attribute_delay_loop:
                subq.b  #1,POSTFLIGHT_DELAY_BYTE.l
                beq.s   postflight_attribute_delay_loop
                bra.s   postflight_attribute_done
postflight_attribute_set_delay:
                move.b  #$1E,POSTFLIGHT_DELAY_BYTE.l
postflight_attribute_done:
