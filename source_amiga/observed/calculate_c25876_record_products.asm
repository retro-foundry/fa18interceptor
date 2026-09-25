; Byte-exact observed record product calculation $C25876-$C258C7.

                org     $C25876

C45BFC                         equ     $C45BFC
C459CA                         equ     $C459CA

calculate_c25876_record_products:
                move.w  d7,d5
                move.w  d7,-(sp)
                lea     C45BFC.l,a0
                movea.l C459CA.l,a3
                move.w  d2,d5
                move.w  d3,d6
                move.w  d4,d7
                muls.w  (a0)+,d5
                muls.w  (a0)+,d6
                muls.w  (a0)+,d7
                add.l   d6,d7
                add.l   d5,d7
                asr.l   #8,d7
                asl.l   d1,d7
                move.l  d7,(a3)+
                clr.l   (a3)+
                addq.w  #6,a0
                muls.w  (a0)+,d2
                muls.w  (a0)+,d3
                muls.w  (a0)+,d4
                add.l   d3,d4
                add.l   d2,d4
                asr.l   #8,d4
                asl.l   d1,d4
                move.l  d4,(a3)+
                move.w  (sp)+,d7
                move.w  d7,(a3)+
                addq.w  #2,a3
                clr.l   (a3)
                clr.l   4(a3)
                clr.l   8(a3)
                move.l  a3,C459CA.l
                rts
