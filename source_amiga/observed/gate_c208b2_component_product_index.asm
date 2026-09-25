; Byte-exact observed product-index gate $C208B2-$C208BF.

                org     $C208B2

gate_c208b2_component_product_index:
                move.w  -$28(a6),d0
                asr.w   #4,d0
                cmpi.w  #$a,d0
                ble.b   $C208C2
