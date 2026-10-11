; Byte-exact observed activity gate at $C30A00-$C30A07.
; The nonpositive route returns through the preceding $C309FE tail.

                org     $C30A00

ACTIVITY_FLAG_C45843           equ     $C45843
ACTIVITY_FLAG_RETURN            equ     $C309FE

gate_c30a00_activity_flag:
                tst.b   ACTIVITY_FLAG_C45843.l
                ble.b   ACTIVITY_FLAG_RETURN
