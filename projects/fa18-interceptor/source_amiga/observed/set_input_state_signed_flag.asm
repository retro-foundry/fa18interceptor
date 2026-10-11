; Byte-exact input-state flag bridge $C1C224-$C1C23B.

                org     $C1C224

INPUT_STATE_SIGNED_FLAG          equ $C4582A
INPUT_STATE_POSITIVE_VALUE       equ 1
INPUT_STATE_NEGATIVE_VALUE       equ $FF
SHARED_COMMAND_FALLBACK          equ $C1C23C

set_input_state_signed_flag:
                tst.b   d6
                beq.s   .store_positive
                bra.s   .store_negative
.store_positive:
                move.b  #INPUT_STATE_POSITIVE_VALUE,INPUT_STATE_SIGNED_FLAG.l
                bra.s   SHARED_COMMAND_FALLBACK
.store_negative:
                move.b  #INPUT_STATE_NEGATIVE_VALUE,INPUT_STATE_SIGNED_FLAG.l
