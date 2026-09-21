; Byte-exact two-bit control-field helper $C1B4D0-$C1B4F3.

                org     $C1B4D0

CONTROL_FIELD                    equ $C461E9
CONTROL_FIELD_LOW2_CLEAR         equ $FC
CONTROL_FIELD_VALUE_ZERO         equ 0
CONTROL_FIELD_VALUE_ONE          equ 1
CONTROL_FIELD_VALUE_TWO          equ 2
CONTROL_RESET_FLAG               equ $C45870

write_two_bit_control_value_one:
                moveq   #CONTROL_FIELD_VALUE_ONE,d2
                bra.b   write_two_bit_control_field

write_two_bit_control_value_two:
                moveq   #CONTROL_FIELD_VALUE_TWO,d2
                bra.b   write_two_bit_control_field

clear_control_reset_and_write_zero:
                clr.b   CONTROL_RESET_FLAG.l
write_two_bit_control_value_zero:
                moveq   #CONTROL_FIELD_VALUE_ZERO,d2
write_two_bit_control_field:
                move.b  CONTROL_FIELD.l,d1
                andi.b  #CONTROL_FIELD_LOW2_CLEAR,d1
                or.b    d2,d1
                move.b  d1,CONTROL_FIELD.l
                rts
