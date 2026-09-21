; Byte-exact geometry midpoint preparation $C20954-$C20983.

                org     $C20954

prepare_c20954_geometry_midpoint:
                movem.w -64(a6),d4-d6
                asr.w   #1,d4
                asr.w   #1,d5
                asr.w   #1,d6
                movem.w (a3),d1-d3
                sub.w   d4,d1
                sub.w   d5,d2
                sub.w   d6,d3
                add.w   -70(a6),d1
                add.w   -68(a6),d2
                add.w   -66(a6),d3
                movem.w d1-d3,-76(a6)
                dc.w    $4284                   ; clr.l   d4
                dc.w    $4285                   ; clr.l   d5
                dc.w    $4286                   ; clr.l   d6
                bra.b   $C20996
