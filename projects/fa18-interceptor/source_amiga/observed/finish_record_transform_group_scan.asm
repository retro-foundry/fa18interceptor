; Byte-exact observed record-transform group-scan continuation $C1EAD8-$C1EAE7.
; The nonnegative component-product path sets the record word's observed
; $8000 flag, increments the shared transform count, and returns to the scan.

                org     $C1EAD8

TRANSFORM_GROUP_COUNT          equ     $C4FD5E

finish_record_transform_group_scan:
                ori.w   #$8000,$2(a1)
                addq.w  #1,TRANSFORM_GROUP_COUNT.l
                bra.w   $C1E652
