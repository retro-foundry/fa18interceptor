; Byte-exact observed C13D84 continuation $C14F8A-$C14F97.
; It loads the shared state word and tests bit 1, continuing only while that
; observed state bit is set.

                org     $C14F8A

CONTROL_UPDATE_STATE           equ     $C458D2

gate_c13d84_shared_state_bit1:
                move.w  CONTROL_UPDATE_STATE.l,d0
                dc.w    $0800,$0001             ; btst.b #1,d0
                beq.w   $C15028
