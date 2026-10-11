; Byte-exact observed raw-$12 command slice $C1B126-$C1B15D.
; Reached by the sealed run002 Shift+E eject event.

                org     $C1B126

FLIGHT_STATE_BYTE              equ $C457AE
EJECT_COMMAND_MODE             equ $C45842
EJECT_COMMAND_LATCH            equ $C457AB
COMMAND_REQUEST_FLAGS          equ $C4599A
CONTROL_COMMAND_FLAGS          equ $C46200

EJECT_COMMAND_MODE_VALUE       equ 8
EJECT_REQUEST_BIT              equ 5
EJECT_CONTROL_FLAG_BITS        equ $0A

SHARED_COMMAND_FALLBACK        equ $C1C23C
SET_COMMAND_LATCH              equ $C1C214

dispatch_eject_command:
                tst.b   d6
                bne.s   .flight_state_check
.zero_d6:
                bra.w   SHARED_COMMAND_FALLBACK
.flight_state_check:
                tst.b   FLIGHT_STATE_BYTE.l
                bne.w   SHARED_COMMAND_FALLBACK
                move.b  #EJECT_COMMAND_MODE_VALUE,EJECT_COMMAND_MODE.l
                lea.l   EJECT_COMMAND_LATCH.l,a0
                bsr.w   SET_COMMAND_LATCH
                bset.b  #EJECT_REQUEST_BIT,COMMAND_REQUEST_FLAGS.l
                ori.b   #EJECT_CONTROL_FLAG_BITS,CONTROL_COMMAND_FLAGS.l
                bra.w   SHARED_COMMAND_FALLBACK
