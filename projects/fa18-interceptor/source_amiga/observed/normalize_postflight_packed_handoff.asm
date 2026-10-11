; Byte-exact static-only packed-value handoff $C33476-$C334A1.
                org     $C33476
POSTFLIGHT_PACKED_VALUE equ $C45B22
POSTFLIGHT_VALUE_STORE equ $C45B1E
POSTFLIGHT_HELPER equ $C259C2
normalize_postflight_packed_handoff:
                lsr.l   #4,d0
                move.l  d0,POSTFLIGHT_PACKED_VALUE.l
                move.l  POSTFLIGHT_PACKED_VALUE.l,-(a7)
                move.l  d1,POSTFLIGHT_PACKED_VALUE.l
                jsr     POSTFLIGHT_HELPER.l
                move.l  (a7)+,POSTFLIGHT_PACKED_VALUE.l
                move.l  POSTFLIGHT_VALUE_STORE.l,d1
                add.l   d2,d1
                divs.w  #3,d1
