; Byte-exact observed activity-mode gate at $C31EB6-$C31EBF.
; The nonzero continuation is outside this bounded slice.

                org     $C31EB6

ACTIVITY_MODE_C45785           equ     $C45785
ACTIVITY_MODE_C31EB6_SKIP      equ     $C31F48

gate_c31eb6_activity_mode:
                tst.b   ACTIVITY_MODE_C45785.l
                beq.w   ACTIVITY_MODE_C31EB6_SKIP
