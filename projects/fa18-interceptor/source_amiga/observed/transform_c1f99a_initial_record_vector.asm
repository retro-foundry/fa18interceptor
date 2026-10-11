; Byte-exact positive-record initial vector transform $C1F9EE-$C1FA3D.
; Matrix/output ownership remains structural.

                org     $C1F9EE

transform_c1f99a_initial_record_vector:
                movem.w (a1)+,d2-d4
                move.w  -8(a6),d7
                asr.w   d7,d2
                asr.w   d7,d3
                asr.w   d7,d4
                lea     (a4),a2
                move.w  d2,d5
                move.w  d3,d6
                move.w  d4,d7
                muls.w  (a2)+,d5
                muls.w  (a2)+,d6
                muls.w  (a2)+,d7
                add.l   d6,d7
                add.l   d5,d7
                asr.l   #8,d7
                movea.w d7,a5
                move.w  d2,d5
                move.w  d3,d6
                move.w  d4,d7
                muls.w  (a2)+,d5
                muls.w  (a2)+,d6
                muls.w  (a2)+,d7
                add.l   d6,d7
                add.l   d5,d7
                asr.l   #8,d7
                muls.w  (a2)+,d2
                muls.w  (a2)+,d3
                muls.w  (a2)+,d4
                add.l   d3,d4
                add.l   d2,d4
                asr.l   #8,d4
                btst.b  #0,-127(a6)
                beq.b   $C1FA3E
