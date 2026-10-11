; Byte-exact observed C13D84 signed-local difference gate $C1437E-$C14393.
; It computes the first-minus-second signed-local difference; values below
; $150 reduce the paired local by five before the shared adjustment path.

                org     $C1437E

gate_c13d84_signed_local_difference:
                move.w  -$16(a6),d0
                ext.l   d0
                move.w  -$1C(a6),d1
                ext.l   d1
                sub.l   d1,d0
                cmpi.l  #$150,d0
                bge.b   $C1439A
