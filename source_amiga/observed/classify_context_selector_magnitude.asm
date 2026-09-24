; Byte-exact selector-magnitude classifier $C1C7F6-$C1C85D.
; Input A3 is supplied by the caller.  The output code at C458BC is structural.

                org     $C1C7F6

SELECTOR_COMPONENT_A            equ     $56
SELECTOR_COMPONENT_B            equ     $58
SELECTOR_COMPONENT_C            equ     $5A
SELECTOR_SECONDARY_COMPONENT    equ     $6C
SELECTOR_COMPONENT_C_SHIFT      equ     2
SELECTOR_MAGNITUDE_LOW          equ     $0060
SELECTOR_MAGNITUDE_HIGH         equ     $00C0
SELECTOR_SECONDARY_LIMIT        equ     $1000
SELECTOR_CLASS_OUTPUT            equ     $C458BC
SELECTOR_CLASS_LOW               equ     5
SELECTOR_CLASS_MIDDLE            equ     3
SELECTOR_CLASS_HIGH              equ     1

classify_context_selector_magnitude:
                move.w  SELECTOR_COMPONENT_A(a3),d0
                bge.b   classify_context_selector_magnitude_a_absolute
                neg.w   d0
classify_context_selector_magnitude_a_absolute:
                move.w  SELECTOR_COMPONENT_B(a3),d1
                bge.b   classify_context_selector_magnitude_b_absolute
                neg.w   d1
classify_context_selector_magnitude_b_absolute:
                move.w  SELECTOR_COMPONENT_C(a3),d2
                bge.b   classify_context_selector_magnitude_c_absolute
                neg.w   d2
classify_context_selector_magnitude_c_absolute:
                asr.w   #SELECTOR_COMPONENT_C_SHIFT,d2
                cmp.w   d0,d1
                bgt.b   classify_context_selector_magnitude_b_larger
                cmp.w   d0,d2
                ble.b   classify_context_selector_magnitude_max_ready
                move.w  d2,d0
                bra.b   classify_context_selector_magnitude_max_ready
classify_context_selector_magnitude_b_larger:
                cmp.w   d2,d1
                bgt.b   classify_context_selector_magnitude_b_maximum
                move.w  d2,d0
                bra.b   classify_context_selector_magnitude_max_ready
classify_context_selector_magnitude_b_maximum:
                move.w  d1,d0
classify_context_selector_magnitude_max_ready:
                cmpi.w  #SELECTOR_MAGNITUDE_LOW,d0
                bgt.b   classify_context_selector_magnitude_over_low
                move.w  SELECTOR_SECONDARY_COMPONENT(a3),d1
                bge.b   classify_context_selector_magnitude_secondary_absolute
                neg.w   d1
classify_context_selector_magnitude_secondary_absolute:
                cmpi.w  #SELECTOR_SECONDARY_LIMIT,d1
                bgt.b   classify_context_selector_magnitude_middle
                move.b  #SELECTOR_CLASS_LOW,SELECTOR_CLASS_OUTPUT.l
                rts
classify_context_selector_magnitude_over_low:
                cmpi.w  #SELECTOR_MAGNITUDE_HIGH,d0
                bgt.b   classify_context_selector_magnitude_high
classify_context_selector_magnitude_middle:
                move.b  #SELECTOR_CLASS_MIDDLE,SELECTOR_CLASS_OUTPUT.l
                rts
classify_context_selector_magnitude_high:
                move.b  #SELECTOR_CLASS_HIGH,SELECTOR_CLASS_OUTPUT.l
                rts
