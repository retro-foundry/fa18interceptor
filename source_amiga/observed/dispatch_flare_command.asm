; Byte-exact raw-$23 command slice $C1C0E4-$C1C121.
; Reached after the D6=0 guard in the sealed run002 documented F/flare event.

                org     $C1C0E4

COMMAND_STATUS_FLAGS           equ $C4599B
FLARE_REQUEST_BIT               equ 1
FLARE_COUNTER                   equ $C4584D
FLARE_ACTIVE_TIMER              equ $C4584F
FLARE_COMMAND_LATCH             equ $C4588B
FLARE_ACTIVE_TIMER_VALUE        equ $1E
FLARE_ACTION_CODE               equ $4026
FLARE_EMPTY_CODE                equ $4027

COMMAND_SIDE_EFFECT             equ $C25704
SHARED_COMMAND_FALLBACK         equ $C1C23C

dispatch_flare_command:
                bset.b  #FLARE_REQUEST_BIT,COMMAND_STATUS_FLAGS.l
                subq.b  #1,FLARE_COUNTER.l
                ble.s   .counter_exhausted
                move.b  #FLARE_ACTIVE_TIMER_VALUE,FLARE_ACTIVE_TIMER.l
                move.w  d0,d4
                move.w  #FLARE_ACTION_CODE,d0
                move.b  #2,FLARE_COMMAND_LATCH.l
                bra.s   .submit_effect
.counter_exhausted:
                clr.b   FLARE_COUNTER.l
                move.w  #FLARE_EMPTY_CODE,d0
.submit_effect:
                jsr     COMMAND_SIDE_EFFECT.l
                move.w  d4,d0
                bra.w   SHARED_COMMAND_FALLBACK
