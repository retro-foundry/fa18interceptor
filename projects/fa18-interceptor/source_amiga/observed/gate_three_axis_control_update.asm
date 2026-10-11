; Byte-exact gate for bounded three-axis control update $C1B3EA-$C1B40F.

                org     $C1B3EA

CONTROL_ENABLE_BIT_OFFSET       equ $04
CONTROL_ENABLE_BIT              equ 4
CONTROL_GUARD_WORD_OFFSET       equ $4C
CONTROL_AXIS_X_OFFSET            equ $28
CONTROL_AXIS_Y_OFFSET            equ $29
CONTROL_AXIS_Z_OFFSET            equ $2A

UPDATE_THREE_AXIS_CONTROL_BYTES equ $C1B410

gate_three_axis_control_update:
                clr.b   d4
                btst.b  #CONTROL_ENABLE_BIT,CONTROL_ENABLE_BIT_OFFSET(a1)
                beq.b   UPDATE_THREE_AXIS_CONTROL_BYTES
                tst.w   CONTROL_GUARD_WORD_OFFSET(a1)
                bge.b   .clear_axes
                bclr.b  #CONTROL_ENABLE_BIT,CONTROL_ENABLE_BIT_OFFSET(a1)
                bra.b   UPDATE_THREE_AXIS_CONTROL_BYTES
.clear_axes:
                move.b  d4,CONTROL_AXIS_X_OFFSET(a1)
                move.b  d4,CONTROL_AXIS_Y_OFFSET(a1)
                move.b  d4,CONTROL_AXIS_Z_OFFSET(a1)
                rts
