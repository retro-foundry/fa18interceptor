; Byte-exact observed matrix-adjustment continuation $C2DDD0-$C2DDDB.
; The prior positive route falls through to this lower-bound check; values
; below $FFF9 take its alternate route, otherwise D1 is set to $FF.

                org     $C2DDD0

clamp_matrix_adjustment_delta_lower:
                bgt.b   $C2DDDC
                cmpi.w  #$fff9,d1
                blt.b   $C2DDE6
                moveq   #-$1,d1
                bra.b   $C2DDE8
