; Byte-exact difference-workspace preparation $C20A80-$C20AE3.
; This code remains inside the enclosing record-walker frame.

                org     $C20A80

prepare_c20a52_projection_differences:
                movem.l a1-a2/a5,-(sp)
                movem.w (a3)+,d1-d6
                lea     $6(a3),a4
                sub.w   (a3),d1
                sub.w   $2(a3),d2
                sub.w   $4(a3),d3
                sub.w   (a3),d4
                sub.w   $2(a3),d5
                sub.w   $4(a3),d6
                movem.w d1-d6,-$46(a6)
                asr.w   #1,d4
                asr.w   #1,d5
                asr.w   #1,d6
                movem.w (a3),d1-d3
                sub.w   d4,d1
                sub.w   d5,d2
                sub.w   d6,d3
                add.w   -$46(a6),d1
                add.w   -$44(a6),d2
                add.w   -$42(a6),d3
                movem.w d1-d3,-$4c(a6)
                movem.w (a4),d1-d3
                sub.w   d4,d1
                sub.w   d5,d2
                sub.w   d6,d3
                add.w   -$46(a6),d1
                add.w   -$44(a6),d2
                add.w   -$42(a6),d3
                movem.w d1-d3,-$52(a6)
