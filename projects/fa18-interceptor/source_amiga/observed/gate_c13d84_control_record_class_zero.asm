; Byte-exact observed C13D84 active-record class-zero gate $C14CCA-$C14CDB.
; The high nibble of active-record byte +$62 selects the following alternate
; route when nonzero; zero takes the local threshold assignment continuation.

                org     $C14CCA

CURRENT_CONTROL_RECORD          equ     $C18210

gate_c13d84_control_record_class_zero:
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.b  $62(a0),d0
                andi.b  #$F0,d0
                tst.b   d0
                bne.b   $C14CE6
