; Byte-exact record-stream skip $C1FEF2-$C1FF09.
; DBF tests before ADDA: advances A2 by $34 * ($C458DA.w & $F), not count+1.

                org     $C1FEF2

C458DA                         equ     $C458DA

adjust_c1fef2_record_pointer:
                moveq   #$34,d0
                move.w  C458DA.l,d1
                andi.w  #$f,d1
                bra.b   .loop_test
.loop:
                adda.w  d0,a2
.loop_test:
                dbf     d1,.loop
                moveq   #0,d0
                rts
