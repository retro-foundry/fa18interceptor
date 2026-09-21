; Byte-exact record-limit gate $C230B0-$C230B7.
; The nonnegative path remains outside this slice.

                org     $C230B0

RECORD_LIMIT_WORD               equ $C459C0

gate_c230b0_record_limit:
                move.w  RECORD_LIMIT_WORD.l,d0
                blt.b   $C230E6
