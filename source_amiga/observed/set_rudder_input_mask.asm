; Byte-exact raw-$38/$39 rudder-command slice $C1B58E-$C1B5B7.
; Reached by sealed run003 comma and period held-input events.

                org     $C1B58E

RUDDER_LEFT_MASK                equ $80
RUDDER_RIGHT_MASK               equ $40
RUDDER_INPUT_MASK               equ $C461E9
RUDDER_INPUT_DIRECTION          equ $C4582F
RUDDER_INPUT_BITS_MASK          equ $3F
SHARED_COMMAND_FALLBACK         equ $C1C23C

set_left_rudder_input:
                move.b  #RUDDER_LEFT_MASK,d2
                bra.s   set_rudder_input_mask
set_right_rudder_input:
                move.b  #RUDDER_RIGHT_MASK,d2
                bra.s   set_rudder_input_mask
clear_rudder_input:
                moveq   #0,d2
set_rudder_input_mask:
                move.b  RUDDER_INPUT_MASK.l,d1
                andi.b  #RUDDER_INPUT_BITS_MASK,d1
                or.b    d2,d1
                move.b  d1,RUDDER_INPUT_MASK.l
                move.b  d2,RUDDER_INPUT_DIRECTION.l
                bra.w   SHARED_COMMAND_FALLBACK
