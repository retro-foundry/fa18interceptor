; Byte-exact static-only small-variant delta gate $C33DFE-$C33E65.
                org     $C33DFE
POSTFLIGHT_VARIANT_PAIR         equ $C4593A
POSTFLIGHT_STATUS_SOURCE        equ $C45B50
POSTFLIGHT_SMALL_VALUE_ENTRY    equ $C33ED8
POSTFLIGHT_DELTA_REJECT_ENTRY   equ $C33EE6
POSTFLIGHT_FALLBACK_ENTRY       equ $C33F52
gate_postflight_small_variant_deltas:
                cmpi.w  #$DE,d0
                bge.w   POSTFLIGHT_SMALL_VALUE_ENTRY
                cmpi.w  #$2E,d1
                ble.w   POSTFLIGHT_SMALL_VALUE_ENTRY
                cmpi.w  #$86,d1
                bge.w   POSTFLIGHT_SMALL_VALUE_ENTRY
                movem.w POSTFLIGHT_VARIANT_PAIR.l,d2-d3
                sub.w   d0,d2
                bge.s   postflight_small_first_delta_ready
                neg.w   d2
postflight_small_first_delta_ready:
                cmpi.w  #5,d2
                bgt.w   POSTFLIGHT_DELTA_REJECT_ENTRY
                sub.w   d1,d3
                bge.s   postflight_small_second_delta_ready
                neg.w   d3
postflight_small_second_delta_ready:
                cmpi.w  #5,d3
                bgt.w   POSTFLIGHT_DELTA_REJECT_ENTRY
                moveq   #1,d4
                ori.l   #$4000,POSTFLIGHT_STATUS_SOURCE.l
                andi.l  #$FFFFFDFF,POSTFLIGHT_STATUS_SOURCE.l
                move.w  #$2700,d7
                cmpi.b  #$30,d5
                beq.s   postflight_small_threshold_ready
                move.w  #$1800,d7
                cmpi.b  #$20,d5
                bne.w   POSTFLIGHT_FALLBACK_ENTRY
postflight_small_threshold_ready:
