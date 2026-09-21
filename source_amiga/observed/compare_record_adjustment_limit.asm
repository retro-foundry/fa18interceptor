; Byte-exact observed record-limit comparison $C2DE1C-$C2DE29.

                org     $C2DE1C

compare_record_adjustment_limit:
                move.w  $50(a1),$52(a1)
                cmp.w   $7e(a1),d4
                ble.b   $C2DE2C
