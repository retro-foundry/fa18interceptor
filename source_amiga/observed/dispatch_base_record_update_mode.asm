; Byte-exact base-record update mode dispatch $C241B4-$C241C7.

                org     $C241B4

dispatch_base_record_update_mode:
                move.b  $05(a1),d0
                beq.b   $C241E4
                cmpi.b  #1,d0
                beq.b   $C241E4
                cmpi.b  #6,d0
                beq.w   $C242D4
