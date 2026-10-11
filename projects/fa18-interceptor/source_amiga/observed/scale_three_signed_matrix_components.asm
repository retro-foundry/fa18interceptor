; Byte-exact observed three-component signed scaling return $C2E334-$C2E345.
; It sign-extends D4-D6, scales each by eight, restores the caller's saved
; registers, and returns.

                org     $C2E334

scale_three_signed_matrix_components:
                ext.l   d4
                asl.l   #3,d4
                ext.l   d5
                asl.l   #3,d5
                ext.l   d6
                asl.l   #3,d6
                movem.l (a7)+,d1/a0-a1
                rts
