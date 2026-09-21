; Byte-exact raw-$33 command slice $C1C172-$C1C1AF.
; Reached by the sealed run002 documented C/chaff event.

                org     $C1C172

COMMAND_STATUS_FLAGS           equ $C4599B
CHAFF_REQUEST_BIT               equ 2
CHAFF_COUNTER                   equ $C4584C
CHAFF_ACTIVE_TIMER              equ $C4584E
CHAFF_COMMAND_LATCH             equ $C4588B
CHAFF_ACTIVE_TIMER_VALUE        equ $1E
CHAFF_ACTION_CODE               equ $4028
CHAFF_EMPTY_CODE                equ $4029

COMMAND_SIDE_EFFECT             equ $C25704
SHARED_COMMAND_FALLBACK         equ $C1C23C

dispatch_chaff_command:
                bset.b  #CHAFF_REQUEST_BIT,COMMAND_STATUS_FLAGS.l
                subq.b  #1,CHAFF_COUNTER.l
                ble.s   .counter_exhausted
                move.b  #CHAFF_ACTIVE_TIMER_VALUE,CHAFF_ACTIVE_TIMER.l
                move.w  d0,d4
                move.w  #CHAFF_ACTION_CODE,d0
                move.b  #1,CHAFF_COMMAND_LATCH.l
                bra.s   .submit_effect
.counter_exhausted:
                clr.b   CHAFF_COUNTER.l
                move.w  #CHAFF_EMPTY_CODE,d0
.submit_effect:
                jsr     COMMAND_SIDE_EFFECT.l
                move.w  d4,d0
                bra.w   SHARED_COMMAND_FALLBACK
