; Byte-exact command-word publisher $C25704-$C25749.
; Reached by sealed run002 chaff/flare and run003 hook command paths.

                org     $C25704

PUBLISHED_COMMAND_WORD          equ $C45AE0
COMMAND_PENDING_FLAGS           equ $C458CC
COMMAND_PENDING_BIT             equ 0
COMMAND_TYPE_FLAGS              equ $C458CE
COMMAND_TYPE_2000_BIT           equ 13
COMMAND_TYPE_4000_BIT           equ 14
COMMAND_TYPE_CLEAR_2000_MASK    equ $7FFF
COMMAND_TYPE_CLEAR_4000_MASK    equ $DFFF
COMMAND_TYPE_HIGH_MASK           equ $FF00
COMMAND_TYPE_2000_MASK           equ $2000
COMMAND_TYPE_4000                equ $4000
COMMAND_TYPE_4800                equ $4800

publish_command_word_flags:
                move.w  d1,-(a7)
                move.w  d0,PUBLISHED_COMMAND_WORD.l
                ori.w   #(1<<COMMAND_PENDING_BIT),COMMAND_PENDING_FLAGS.l
                andi.w  #COMMAND_TYPE_HIGH_MASK,d0
                move.w  d0,d1
                andi.w  #COMMAND_TYPE_2000_MASK,d1
                beq.s   .check_4000_or_4800
                andi.w  #COMMAND_TYPE_CLEAR_2000_MASK,COMMAND_TYPE_FLAGS.l
                ori.w   #COMMAND_TYPE_2000_MASK,COMMAND_TYPE_FLAGS.l
                bra.s   .restore_d1
.check_4000_or_4800:
                cmpi.w  #COMMAND_TYPE_4000,d0
                beq.s   .clear_4000_flag
                cmpi.w  #COMMAND_TYPE_4800,d0
                bne.s   .restore_d1
.clear_4000_flag:
                andi.w  #COMMAND_TYPE_CLEAR_4000_MASK,COMMAND_TYPE_FLAGS.l
.restore_d1:
                move.w  (a7)+,d1
                rts
