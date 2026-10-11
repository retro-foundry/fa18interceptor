; Byte-exact observed C13D84 continuation $C14114-$C1411F.
; It copies one shared control accumulator word to its paired storage, then
; joins signed-term preparation.

                org     $C14114

CONTROL_ACCUMULATOR            equ     $C45778
CONTROL_ACCUMULATOR_COMPANION  equ     $C4577C

copy_c13d84_control_accumulator:
                move.w  CONTROL_ACCUMULATOR.l,CONTROL_ACCUMULATOR_COMPANION.l
                bra.b   $C1414E
