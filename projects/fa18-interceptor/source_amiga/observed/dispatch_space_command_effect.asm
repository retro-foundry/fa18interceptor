; Byte-exact Space-command helper $C0833E-$C08393.
; Called by the sealed run002 raw-$40 dispatch path at $C1B21C.

                org     $C0833E

CONTROL_COMMAND_FLAGS          equ $C46200
COMMAND_MODE_BYTE              equ $C458A6
COMMAND_STATUS_BYTE            equ $C4579A
SECONDARY_REQUEST_FLAGS        equ $C46986
COMMAND_REQUEST_FLAGS          equ $C4599A
SPACE_MODE_BYTE                equ $C461E7
SPACE_COMMAND_LATCH            equ $C457BA
SPACE_COMMAND_WORD             equ $C458C6

LOW_NIBBLE_MASK                equ $0F
HIGH_NIBBLE_MASK               equ $F0
MODE_7D                        equ $7D

dispatch_space_command_effect:
                move.b  CONTROL_COMMAND_FLAGS.l,d4
                andi.b  #LOW_NIBBLE_MASK,d4
                bne.s   .early_return
                cmpi.b  #MODE_7D,COMMAND_MODE_BYTE.l
                bne.s   .set_request
                tst.b   COMMAND_STATUS_BYTE.l
                bge.s   .early_return
                bset.b  #3,SECONDARY_REQUEST_FLAGS.l
.early_return:
                rts
.set_request:
                bset.b  #2,COMMAND_REQUEST_FLAGS.l
                move.b  SPACE_MODE_BYTE.l,d4
                andi.b  #HIGH_NIBBLE_MASK,d4
                beq.s   .late_return
                cmpi.b  #$10,d4
                beq.s   .set_word_flag
                move.b  #1,SPACE_COMMAND_LATCH.l
.late_return:
                rts
.set_word_flag:
                ori.w   #8,SPACE_COMMAND_WORD.l
                rts
