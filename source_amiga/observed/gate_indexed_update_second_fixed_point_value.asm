; Byte-exact second fixed-point gate $C25EB8-$C25EC1.

                org     $C25EB8

gate_indexed_update_second_fixed_point_value:
                swap    d1
                asr.w   #6,d1
                cmp.w   $08(a1),d1
                dc.w    $6706                   ; beq.b $C25EC8
