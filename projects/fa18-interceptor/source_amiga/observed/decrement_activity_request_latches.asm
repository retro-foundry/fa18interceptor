; Byte-exact observed activity request-latch decrements $C253D2-$C253FD.
; It decrements three nonnegative byte latches through the shared helper, then
; loads one latch and tests a companion byte for the following bounded update.

                org     $C253D2

ACTIVITY_LATCH_A                equ     $C45886
ACTIVITY_LATCH_B                equ     $C45891
ACTIVITY_LATCH_C                equ     $C4588A
ACTIVITY_LATCH_VALUE            equ     $C45889
ACTIVITY_LATCH_ENABLE           equ     $C45888
DECREMENT_NONNEGATIVE_BYTE      equ     $C25482

decrement_activity_request_latches:
                lea     ACTIVITY_LATCH_A.l,a0
                bsr.w   DECREMENT_NONNEGATIVE_BYTE
                lea     ACTIVITY_LATCH_B.l,a0
                bsr.w   DECREMENT_NONNEGATIVE_BYTE
                lea     ACTIVITY_LATCH_C.l,a0
                bsr.w   DECREMENT_NONNEGATIVE_BYTE
                move.b  ACTIVITY_LATCH_VALUE.l,d1
                tst.b   ACTIVITY_LATCH_ENABLE.l
                beq.b   $C2540A
