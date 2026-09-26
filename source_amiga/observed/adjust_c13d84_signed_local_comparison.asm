; Byte-exact observed C13D84 signed-local comparison branch $C1439A-$C143B3.
; It subtracts $2A from one local, negates it into another, then compares that
; value against the paired signed local before the shared continuation.

                org     $C1439A

adjust_c13d84_signed_local_comparison:
                subi.w  #$2A,-$18(a6)
                move.w  -$18(a6),d0
                neg.w   d0
                move.w  d0,-$1C(a6)
                move.w  -$16(a6),d1
                cmp.w   d1,d0
                ble.w   $C146C2
