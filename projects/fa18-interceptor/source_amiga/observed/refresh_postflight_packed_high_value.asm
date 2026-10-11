; Byte-exact static-only packed-high refresh $C3345C-$C33475.
                org     $C3345C
POSTFLIGHT_VALUE_STORE equ $C45B1E
POSTFLIGHT_PACKED_VALUE equ $C45B22
POSTFLIGHT_HELPER equ $C25A08
refresh_postflight_packed_high_value:
                addi.l  #$64,POSTFLIGHT_VALUE_STORE.l
                jsr     POSTFLIGHT_HELPER.l
                move.l  POSTFLIGHT_PACKED_VALUE.l,d0
                andi.w  #$FF00,d0
