; Byte-exact static map-transition tail $C1BFBC-$C1C051.
; This range follows the capped raw-$37 M command helper; execution is not
; claimed beyond the helper return.

                org     $C1BFBC

MAP_TRANSITION_VECTOR_A         equ $10C00000
MAP_TRANSITION_VECTOR_B         equ $11400000
MAP_TRANSITION_COMPONENT_MASK   equ $003FFFFF
MAP_TRANSITION_SAVED_VECTOR     equ $C45664
MAP_TRANSITION_VECTOR_A_POS     equ $C45C3E
MAP_TRANSITION_VECTOR_B_POS     equ $C45C42
MAP_TRANSITION_VECTOR_C_POS     equ $C45C46
MAP_TRANSITION_VECTOR_A_NEG     equ $C45C32
MAP_TRANSITION_VECTOR_B_NEG     equ $C45C36
MAP_TRANSITION_VECTOR_C_NEG     equ $C45C3A
MAP_TRANSITION_COUNTER_A        equ $C45C4A
MAP_TRANSITION_COUNTER_B        equ $C45C4E
MAP_TRANSITION_COUNTER_C        equ $C45C52
MAP_TRANSITION_TIMER            equ $C45A94
MAP_TRANSITION_TIMER_INITIAL    equ $1C20
MAP_TRANSITION_TIMER_FRACTION   equ $C45A96
MAP_TRANSITION_FLAG_A           equ $C457B4
MAP_TRANSITION_FLAG_B           equ $C457B5
MAP_TRANSITION_ACTIVE            equ $C457AD
DISPLAY_STATE_GUARD              equ $C45785
MAP_DISPLAY_CODE                 equ $C45984
MAP_DISPLAY_CODE_VALUE           equ $00B3
MAP_TRANSITION_ALTERNATE         equ $C1B69A
SHARED_COMMAND_FALLBACK          equ $C1C23C

initialize_map_transition_state:
                move.w  (a7)+,d0
                move.l  #MAP_TRANSITION_VECTOR_A,d4
                move.l  MAP_TRANSITION_SAVED_VECTOR.l,d1
                move.l  #MAP_TRANSITION_VECTOR_B,d2
                move.l  d4,MAP_TRANSITION_VECTOR_A_POS.l
                move.l  d1,MAP_TRANSITION_VECTOR_B_POS.l
                move.l  d2,MAP_TRANSITION_VECTOR_C_POS.l
                andi.l  #MAP_TRANSITION_COMPONENT_MASK,d4
                andi.l  #MAP_TRANSITION_COMPONENT_MASK,d2
                neg.l   d4
                neg.l   d1
                neg.l   d2
                move.l  d4,MAP_TRANSITION_VECTOR_A_NEG.l
                move.l  d1,MAP_TRANSITION_VECTOR_B_NEG.l
                move.l  d2,MAP_TRANSITION_VECTOR_C_NEG.l
                clr.l   MAP_TRANSITION_COUNTER_A.l
                clr.l   MAP_TRANSITION_COUNTER_B.l
                clr.l   MAP_TRANSITION_COUNTER_C.l
                move.w  #MAP_TRANSITION_TIMER_INITIAL,MAP_TRANSITION_TIMER.l
                clr.w   MAP_TRANSITION_TIMER_FRACTION.l
                clr.b   MAP_TRANSITION_FLAG_A.l
                move.b  #1,MAP_TRANSITION_FLAG_B.l
                move.b  #1,MAP_TRANSITION_ACTIVE.l
                tst.b   DISPLAY_STATE_GUARD.l
                beq.w   MAP_TRANSITION_ALTERNATE
                move.w  #MAP_DISPLAY_CODE_VALUE,MAP_DISPLAY_CODE.l
                bra.w   SHARED_COMMAND_FALLBACK
