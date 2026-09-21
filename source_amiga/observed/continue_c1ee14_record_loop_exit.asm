; Byte-exact record-loop exit continuation $C1F2E4-$C1F2ED.

                org     $C1F2E4

continue_c1ee14_record_loop_exit:
                subq.w  #1,d0
                bgt.w   $C1F21C
                bra.w   $C1F6F8
