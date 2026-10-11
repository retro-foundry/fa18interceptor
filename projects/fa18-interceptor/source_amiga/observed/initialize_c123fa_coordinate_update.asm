; Byte-exact observed entry of coordinate-update helper $C123FA-$C12435.
; The third negative-component branch begins at the unobserved $C12436 path.

                org     $C123FA

coordinate_update_initialize:
                link.w  a6,#-$1c
                movem.l d2,-(sp)
                move.w  #2,-$1c(a6)
                moveq   #0,d0
                move.b  d0,-$9(a6)
                tst.l   $10(a6)
                bpl.b   $C12420
                neg.l   $10(a6)
                bset    #0,d0
                move.b  d0,-$9(a6)
                tst.l   $14(a6)
                bpl.b   $C12430
                neg.l   $14(a6)
                bset    #1,-$9(a6)
                tst.l   $18(a6)
                bpl.b   $C12440
