; Byte-exact observed $C1FB82 entry.  The input and branch ownership are
; structural; the fall-through and target remain outside this slice.
                org $C1FB82
D7_BIT_PAIR_MASK equ $0C00
NONZERO_BIT_PAIR_PATH equ $C1FC3A
branch_on_d7_bit_pair:
 move.w d7,d1
 andi.w #D7_BIT_PAIR_MASK,d1
 bne.w NONZERO_BIT_PAIR_PATH
