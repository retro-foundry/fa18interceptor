; Byte-exact observed basic block $C11D9A-$C11DB1.
; Both following paths are outside this exact gate slice.

                org     $C11D9A

POSTFLIGHT_NEXT_STATE_WORD      equ     $C45AE2
POSTFLIGHT_CURRENT_STATE_PATH   equ     $C11E3C

gate_c11d9a_postflight_state_change:
                move.w  POSTFLIGHT_NEXT_STATE_WORD.l,d0
                andi.w  #$ff,d0
                move.w  -$4(a6),d1
                andi.w  #$ff,d1
                cmp.w   d1,d0
                beq.w   POSTFLIGHT_CURRENT_STATE_PATH
