; Byte-exact observed post-matrix record bit-1 guard $C2D7DC-$C2D7E5.
; With the shared selector word clear, bit 1 of the current record's +$20
; status byte selects the alternate post-matrix route.

                org     $C2D7DC

guard_postmatrix_record_bit1:
                btst.b  #1,$20(a1)
                bne.w   $C2D868
