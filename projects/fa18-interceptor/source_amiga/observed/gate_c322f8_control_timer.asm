; Byte-exact signed-state continuation $C322F8-$C32317.
; Field ownership and scenario role remain unresolved.

                org     $C322F8

CONTROL_MASK_WORD               equ     $C458CC
CONTROL_TIMER_BYTE              equ     $C45887
SIGNED_STATE_ALTERNATE          equ     $C32510

gate_c322f8_control_timer:
                move.w  CONTROL_MASK_WORD.l,d3
                andi.w  #$81,d3
                bne.w   SIGNED_STATE_ALTERNATE
                subq.b  #1,CONTROL_TIMER_BYTE.l
                bge.w   SIGNED_STATE_ALTERNATE
                move.b  #$FF,CONTROL_TIMER_BYTE.l

