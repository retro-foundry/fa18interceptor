; Byte-exact renderer counter gate $C30C20-$C30C27.

                org     $C30C20

gate_renderer_counter_c45840_positive:
                tst.b   $C45840.l
                ble.b   $C30C70
