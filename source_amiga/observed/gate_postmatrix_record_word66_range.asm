; Byte-exact observed post-matrix record +$66 range gate $C2D8A8-$C2D8BB.
; Values at or below $1770 and at or above $5910 take the common matrix-tail
; publication route.  The middle range enters the following comparison path.

                org     $C2D8A8

gate_postmatrix_record_word66_range:
                move.w  $66(a1),d0
                cmpi.w  #$1770,d0
                ble.w   $C2D94E
                cmpi.w  #$5910,d0
                bge.w   $C2D94E
