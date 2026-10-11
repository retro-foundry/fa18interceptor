; Byte-exact command-state fragment $C1C1B0-$C1C1F1.
; The caller/dispatch raw key has not yet been isolated.

                org     $C1C1B0

SHARED_COMMAND_QUEUE           equ $C1C23C
STATE_POINTER_GUARD            equ $C4FDBC
STATE_POINTER_SOURCE_A         equ $C4FDA4
STATE_POINTER_SOURCE_B         equ $C4FDB0
STATE_POINTER_ACTIVE           equ $C4FDB8
COMMAND_MODE_LATCH             equ $C4584B
COMMAND_MODE_VALUE             equ 3
INPUT_STATE_SIGN_FLAG          equ $C4582A
INPUT_STATE_POSITIVE           equ 1

skip_buffered_command_refresh:
                bra.w   SHARED_COMMAND_QUEUE

refresh_buffered_command_state:
                tst.l   STATE_POINTER_GUARD.l
                beq.w   SHARED_COMMAND_QUEUE
                move.l  STATE_POINTER_SOURCE_A.l,STATE_POINTER_GUARD.l
                move.l  STATE_POINTER_SOURCE_B.l,STATE_POINTER_ACTIVE.l
                addq.l  #1,STATE_POINTER_GUARD.l
                addq.l  #8,STATE_POINTER_ACTIVE.l
                move.b  #COMMAND_MODE_VALUE,COMMAND_MODE_LATCH.l
                move.b  #INPUT_STATE_POSITIVE,INPUT_STATE_SIGN_FLAG.l
                bra.w   SHARED_COMMAND_QUEUE
