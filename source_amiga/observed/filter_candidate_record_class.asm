; Byte-exact candidate class filters $C26F38-$C26F51.

                org     $C26F38

filter_candidate_record_class:
                move.b  $62(a0,d0.w),d7
                cmpi.b  #$15,d7
                beq.b   $C26F94
                andi.b  #$f0,d7
                cmpi.b  #$30,d7
                beq.b   $C26F00
                cmpi.b  #$20,d7
                bne.b   $C26F5A
