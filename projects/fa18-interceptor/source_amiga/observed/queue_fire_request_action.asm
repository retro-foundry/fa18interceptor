; Byte-exact command tail $C1BB66-$C1BB79.
; It precedes the raw-$44 weapon-cycle handler in the keyboard dispatcher.

                org     $C1BB66

FIRE_REQUEST_FLAGS             equ $C4599D
FIRE_REQUEST_BIT               equ 6
COMMAND_ACTION_REQUEST         equ $C45891
FIRE_REQUEST_ACTION_VALUE      equ 5
SHARED_COMMAND_QUEUE           equ $C1C23C

queue_fire_request_action:
                bset.b  #FIRE_REQUEST_BIT,FIRE_REQUEST_FLAGS.l
                move.b  #FIRE_REQUEST_ACTION_VALUE,COMMAND_ACTION_REQUEST.l
                bra.w   SHARED_COMMAND_QUEUE
