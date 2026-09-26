; Byte-exact observed C12950 control-record continuation $C12A0A-$C12A13.
; It tests a shared long value and takes the zero-value route to $C12C84.

                org     $C12A0A

CONTROL_RECORD_SHARED_LONG      equ     $C45B54

gate_c12950_shared_long_zero:
                tst.l   CONTROL_RECORD_SHARED_LONG.l
                beq.w   $C12C84
