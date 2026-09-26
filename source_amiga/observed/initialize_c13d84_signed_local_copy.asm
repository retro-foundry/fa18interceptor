; Byte-exact observed C13D84 continuation $C142A6-$C142B1.
; It copies signed local -$24 to local -$26 and branches directly when the
; copied value is nonnegative.

                org     $C142A6

initialize_c13d84_signed_local_copy:
                move.w  -$24(a6),d0
                move.w  d0,-$26(a6)
                tst.w   d0
                bpl.b   $C142B6
