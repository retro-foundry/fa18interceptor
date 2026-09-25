; Byte-exact postflight packed-decimal text formatter $C3267A-$C326B7.

                org     $C3267A

POSTFLIGHT_FORMAT_INPUT         equ     $C45B1E
POSTFLIGHT_PACKED_VALUE         equ     $C45B22
PACKED_BCD_CONVERTER            equ     $C25A08

format_postflight_packed_value:
                move.l  d0,POSTFLIGHT_FORMAT_INPUT.l
                jsr     PACKED_BCD_CONVERTER.l
                move.l  POSTFLIGHT_PACKED_VALUE.l,d3
                move.w  d2,d0
.emit_low_nibble_first:
                move.w  d3,d1
                andi.w  #$F,d1
                addi.w  #$30,d1
                move.b  d1,-(a2)
                lsr.l   #4,d3
                dbra    d2,.emit_low_nibble_first
                tst.b   d4
                bne.b   .return
                subq.w  #1,d0
.blank_leading_zeroes:
                cmpi.b  #$30,(a2)+
                bne.b   .return
                move.b  #$20,-1(a2)
                dbra    d0,.blank_leading_zeroes
.return:
                rts

