; Byte-exact observed post-threshold flag clear $C2D844-$C2D84B.

                org     $C2D844

                dc.w    $08A9,$0002,$0020 ; bclr.b #2,$20(a1); retain original EA
                bra.b   $C2D8A8
