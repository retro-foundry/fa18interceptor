; Byte-exact record-update threshold state selection $C23C82-$C23C8D.

                org     $C23C82

select_record_update_threshold_state:
                cmp.w   $6C(a1),d1
                beq.b   $C23C96
                blt.b   $C23C8E
                moveq   #1,d2
                bra.b   $C23C98
