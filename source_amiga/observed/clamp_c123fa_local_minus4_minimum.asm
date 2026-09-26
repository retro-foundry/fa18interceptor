; Byte-exact observed C123FA local -$4 lower clamp $C12F38-$C12F3F.

                org     $C12F38

clamp_c123fa_local_minus4_minimum:
                cmpi.w  #$28,-$4(a6)
                bge.b   $C12F46
