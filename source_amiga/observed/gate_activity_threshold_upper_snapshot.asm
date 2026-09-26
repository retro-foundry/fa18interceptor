; Byte-exact activity-threshold upper snapshot gate $C254BE-$C254C5.

                org     $C254BE

gate_activity_threshold_upper_snapshot:
                cmp.w   $C458E0.l,d0
                ble.b   $C254CC
