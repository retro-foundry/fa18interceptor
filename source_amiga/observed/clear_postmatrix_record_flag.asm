; Byte-exact observed threshold-failure path $C2D83A-$C2D843.

                org     $C2D83A

clear_postmatrix_record_flag:
                cmp.w   $6e(a1),d0
                bge.b   $C2D84C
                clr.w   $26(a1)
