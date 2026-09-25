; Byte-exact observed activity gate at $C31A64-$C31A6B.
; The nonpositive continuation at $C31AC8 is outside this slice.

                org     $C31A64

ACTIVITY_FLAG_C4583B           equ     $C4583B
ACTIVITY_FLAG_C31A64_SKIP      equ     $C31AC8

gate_c31a64_activity_flag:
                tst.b   ACTIVITY_FLAG_C4583B.l
                ble.b   ACTIVITY_FLAG_C31A64_SKIP
