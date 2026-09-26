; Byte-exact observed control-update countdown gate $C13134-$C13143.
; It clears an input snapshot byte, loads a signed countdown byte, and sends
; nonpositive values to the common epilogue; positive values use the following
; decrement path.

                org     $C13134

INPUT_SNAPSHOT_A                equ     $C458B0
CONTROL_UPDATE_COUNTDOWN        equ     $C45797

gate_control_update_countdown:
                clr.b   INPUT_SNAPSHOT_A
                move.b  CONTROL_UPDATE_COUNTDOWN,d0
                tst.b   d0
                ble.b   $C1316E
