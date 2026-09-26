; Byte-exact activity helper return and byte-sign gate $C25480-$C25485.

                org     $C25480

return_activity_counter_helper:
                rts

gate_activity_counter_byte_sign:
                tst.b   (a0)
                blt.b   $C25488
