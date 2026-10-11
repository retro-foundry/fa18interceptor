; Byte-exact observed matrix-record class-6 gate $C2D4D0-$C2D4D9.
; A record class byte of six joins the shared +$6C threshold path.

                org     $C2D4D0

gate_matrix_record_class6:
                cmpi.b  #6,$5(a1)
                beq.w   $C2D594
