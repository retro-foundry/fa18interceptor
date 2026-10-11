; Byte-exact observed activity-value refresh and companion latch update
; $C253B2-$C253C7.  It commits the new activity value, decrements a byte
; latch, then clears bit 7 and decrements the adjacent signed latch when it is
; nonzero; underflow clamps that latch to zero.

                org     $C253B2

ACTIVITY_VALUE                  equ     $C45B02
ACTIVITY_LATCH_A                equ     $C45884
ACTIVITY_LATCH_B                equ     $C45885

refresh_activity_value_and_latch:
                move.l  d0,ACTIVITY_VALUE
                subq.b  #1,ACTIVITY_LATCH_A
                lea     ACTIVITY_LATCH_B,a0
                tst.b   (a0)
                beq.b   $C253D2
