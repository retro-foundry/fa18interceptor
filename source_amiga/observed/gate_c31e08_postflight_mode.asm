; Byte-exact observed mode gate $C31E08-$C31E0D.

                org     $C31E08

gate_c31e08_postflight_mode:
                cmpi.w  #1,d6
                ble.b   $C31E14
