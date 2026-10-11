; Byte-exact four-entry wrapping-counter helper cluster $C1CA2E-$C1CA81.
; Counter ownership is unassigned; each word wraps inclusively over 0..127.

                org     $C1CA2E

CONTEXT_WRAP_COUNTER_A          equ     $C4618A
CONTEXT_WRAP_COUNTER_B          equ     $C4618C
CONTEXT_WRAP_MAXIMUM            equ     $007F

increment_context_wrap_counter_a:
                addq.w  #1,CONTEXT_WRAP_COUNTER_A.l
                cmpi.w  #CONTEXT_WRAP_MAXIMUM,CONTEXT_WRAP_COUNTER_A.l
                ble.b   increment_context_wrap_counter_a_done
                clr.w   CONTEXT_WRAP_COUNTER_A.l
increment_context_wrap_counter_a_done:
                rts

decrement_context_wrap_counter_a:
                subq.w  #1,CONTEXT_WRAP_COUNTER_A.l
                bge.b   decrement_context_wrap_counter_a_done
                move.w  #CONTEXT_WRAP_MAXIMUM,CONTEXT_WRAP_COUNTER_A.l
decrement_context_wrap_counter_a_done:
                rts

increment_context_wrap_counter_b:
                addq.w  #1,CONTEXT_WRAP_COUNTER_B.l
                cmpi.w  #CONTEXT_WRAP_MAXIMUM,CONTEXT_WRAP_COUNTER_B.l
                ble.b   increment_context_wrap_counter_b_done
                clr.w   CONTEXT_WRAP_COUNTER_B.l
increment_context_wrap_counter_b_done:
                rts

decrement_context_wrap_counter_b:
                subq.w  #1,CONTEXT_WRAP_COUNTER_B.l
                bge.b   decrement_context_wrap_counter_b_done
                move.w  #CONTEXT_WRAP_MAXIMUM,CONTEXT_WRAP_COUNTER_B.l
decrement_context_wrap_counter_b_done:
                rts
