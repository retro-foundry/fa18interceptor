; Byte-exact base-record followup class gate $C2436A-$C24379.

                org     $C2436A

gate_base_record_followup_class:
                move.b  $39(a1),d0
                andi.b  #$F0,d0
                cmpi.b  #$10,d0
                bne.b   $C24368
