; Byte-exact observed C1F6F8 two-component record transform $C1FB24-$C1FB81.
; It positions the output cursor from D7, transforms counted input pairs with
; matrix lanes, and returns to its caller.  Record ownership remains unknown.

                org     $C1FB24

transform_c1f6f8_two_component_records:
                move.w  d7,d2
                asr.w   #1,d2
                add.w   d2,d7
                adda.w  d7,a3
                movem.w -$78(a6),d3/d6
                move.w  (a1)+,d2
                move.w  (a1)+,d4
                move.w  -$8(a6),d7
                asr.w   d7,d2
                asr.w   d7,d4
                add.w   d0,d2
                add.w   d1,d4
                lea     (a4),a0
                move.w  d2,d5
                move.w  d4,d7
                muls.w  (a0)+,d5
                addq.w  #2,a0
                muls.w  (a0)+,d7
                add.l   d5,d7
                asr.l   #8,d7
                add.w   d3,d7
                move.w  d7,(a3)+
                move.w  d2,d5
                move.w  d4,d7
                muls.w  (a0)+,d5
                addq.w  #2,a0
                muls.w  (a0)+,d7
                add.l   d5,d7
                asr.l   #8,d7
                add.w   d6,d7
                move.w  d7,(a3)+
                muls.w  (a0)+,d2
                muls.w  2(a0),d4
                add.l   d2,d4
                asr.l   #8,d4
                add.w   -$74(a6),d4
                move.w  d4,(a3)+
                subq.w  #1,-$a(a6)
                bgt.b   $C1FB32
                movea.l (sp)+,a1
                rts
