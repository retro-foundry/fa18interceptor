; Byte-exact observed eject-command tail $C1B15E-$C1B1A3.

                org     $C1B15E

EJECT_STATE_WORD                 equ $C458D8
EJECT_STATE_CODE                 equ $C45984
EJECT_STATE_SCALED_VALUE         equ $C45918
EJECT_STATE_PENDING_FLAG         equ $C457A9
EJECT_STATE_ZERO_CODE            equ $90
EJECT_STATE_NEGATIVE_CODE        equ $8F
PREPARE_INPUT_HELPER             equ $C1BA86

finish_eject_command_state:
                tst.w   EJECT_STATE_WORD.l
                beq.s   .set_negative_state
                move.w  #0,d1
                move.w  #EJECT_STATE_ZERO_CODE,EJECT_STATE_CODE.l
                bra.s   .store_state
.set_negative_state:
                move.w  #$FFFF,d1
                move.w  #EJECT_STATE_NEGATIVE_CODE,EJECT_STATE_CODE.l
.store_state:
                move.w  d1,EJECT_STATE_WORD.l
                asl.w   #3,d1
                move.w  d1,d3
                add.w   d3,d3
                add.w   d3,d3
                add.w   d3,d1
                ext.l   d1
                move.l  d1,EJECT_STATE_SCALED_VALUE.l
                move.b  #0,EJECT_STATE_PENDING_FLAG.l
                bra.w   PREPARE_INPUT_HELPER
