; Byte-exact sibling difference setup $C20C66-$C20C9B.

                org     $C20C66

prepare_c20c38_projection_differences:
                movem.w (a3)+,d1-d6
                sub.w   $6(a3),d1
                sub.w   $8(a3),d2
                sub.w   $a(a3),d3
                sub.w   $6(a3),d4
                sub.w   $8(a3),d5
                sub.w   $a(a3),d6
                movem.w d1-d6,-$46(a6)
                movem.w (a3)+,d1-d3
                sub.w   (a3),d1
                sub.w   $2(a3),d2
                sub.w   $4(a3),d3
                movem.w d1-d3,-$52(a6)
                clr.w   -$7e(a6)
