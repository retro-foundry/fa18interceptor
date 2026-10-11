; Byte-exact positive-record offset transform and return $C1FA3E-$C1FA91.
; Matrix/output ownership remains structural.

                org     $C1FA3E

transform_c1f99a_offset_record_vector:
                move.w  a5,d2
                move.w  d7,d3
                add.w   d0,d2
                add.w   a0,d3
                add.w   d1,d4
                lea     18(a4),a2
                move.w  d2,d5
                move.w  d3,d6
                move.w  d4,d7
                muls.w  (a2)+,d5
                muls.w  (a2)+,d6
                muls.w  (a2)+,d7
                add.l   d6,d7
                add.l   d5,d7
                asr.l   #8,d7
                move.w  d7,(a3)+
                move.w  d2,d5
                move.w  d3,d6
                move.w  d4,d7
                muls.w  (a2)+,d5
                muls.w  (a2)+,d6
                muls.w  (a2)+,d7
                add.l   d6,d7
                add.l   d5,d7
                asr.l   #8,d7
                move.w  d7,(a3)+
                muls.w  (a2)+,d2
                muls.w  (a2)+,d3
                muls.w  (a2)+,d4
                add.l   d3,d4
                add.l   d2,d4
                asr.l   #8,d4
                move.w  d4,(a3)+
                subq.w  #1,-10(a6)
                bgt.w   $C1F9EE
                movem.l (sp)+,a2/a5
                movea.l (sp)+,a1
                rts
