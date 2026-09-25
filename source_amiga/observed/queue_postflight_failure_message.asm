; Byte-exact postflight failure-message callback $C118A0-$C118E5.
; Run062 takes the $0063 selector branch after its countdown expires.

                org     $C118A0

POSTFLIGHT_COUNTDOWN            equ     $C45AD6
POSTFLIGHT_FAILURE_INPUT         equ     $C45849
POSTFLIGHT_MESSAGE_SEQUENCE      equ     $C4574A
POSTFLIGHT_MODE9_FLAG            equ     $C4582B
POSTFLIGHT_CALLBACK_SLOT         equ     $C1820C
POSTFLIGHT_FAILURE_HELPER_ARG    equ     $C08490
POSTFLIGHT_FAILURE_HELPER        equ     $C11ACC
POSTFLIGHT_FAILURE_STATUS_GATE   equ     $C118E6

queue_postflight_failure_message:
                move.w  POSTFLIGHT_COUNTDOWN.l,d0
                tst.w   d0
                bpl.b   .return
                pea.l   POSTFLIGHT_FAILURE_HELPER_ARG.l
                bsr.w   POSTFLIGHT_FAILURE_HELPER
                addq.l  #4,sp
                move.b  POSTFLIGHT_FAILURE_INPUT.l,d0
                cmpi.b  #$10,d0
                bne.b   .queue_selector_63
                move.w  #$62,POSTFLIGHT_MESSAGE_SEQUENCE.l
                bra.b   .install_status_gate
.queue_selector_63:
                move.w  #$63,POSTFLIGHT_MESSAGE_SEQUENCE.l
.install_status_gate:
                clr.b   POSTFLIGHT_MODE9_FLAG.l
                lea.l   POSTFLIGHT_FAILURE_STATUS_GATE(pc),a0
                move.l  a0,POSTFLIGHT_CALLBACK_SLOT.l
.return:
                rts
