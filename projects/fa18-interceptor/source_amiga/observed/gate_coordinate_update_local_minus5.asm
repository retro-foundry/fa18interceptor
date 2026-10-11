; Byte-exact coordinate-update local -$5 gate $C123E0-$C123E5.

                org     $C123E0

gate_coordinate_update_local_minus5:
                tst.b   -$5(a6)
                beq.b   $C123F6
