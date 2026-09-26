; Byte-exact C13D84 local -$16 upper clamp $C1431E-$C14325.

                org     $C1431E

clamp_c13d84_local_minus16_upper:
                cmpi.w  #$1FEF,-$16(a6)
                ble.b   $C1432C
