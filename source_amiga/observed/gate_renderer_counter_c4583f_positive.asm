; Byte-exact renderer counter gate $C30BC4-$C30BCB.

                org     $C30BC4

gate_renderer_counter_c4583f_positive:
                tst.b   $C4583F.l
                ble.b   $C30C20
