; Byte-exact base-record update followup gate $C241A6-$C241AD.

                org     $C241A6

gate_base_record_update_followup:
                dc.w    $0829,$0001,$0020       ; btst.b #1,$20(a1)
                bne.w   $C242D4
