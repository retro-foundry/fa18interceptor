; Byte-exact base-record update gate $C240E2-$C240EB.

                org     $C240E2

gate_base_record_update_path:
                move.w  $02(a1),d0
                andi.w  #1,d0
                dc.w    $673C                   ; beq.b $C24128
