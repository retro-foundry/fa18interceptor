; Byte-exact map static-packet pair transform $C2AF92-$C2AFF7.
; DETAIL_SHIFT at -$20(A6) is set by $C2AD00 before this loop.

                org     $C2AF92

map_packet_pair_transform:
                move.w  -$20(a6),d3
.next_pair:
                move.l  d0,d2
                swap    d2
                move.w  d0,d4
                add.w   (a3)+,d2
                add.w   (a3)+,d4
                lea     (a0),a1
                move.w  d2,d5
                move.w  d4,d7
                muls.w  (a1)+,d5
                addq.w  #2,a1
                muls.w  (a1)+,d7
                add.l   d5,d7
                asr.l   #8,d7
                add.w   d6,d7
                asl.w   d3,d7
                move.w  d7,(a5)+
                move.w  d2,d5
                move.w  d4,d7
                muls.w  (a1)+,d5
                addq.w  #2,a1
                muls.w  (a1)+,d7
                add.l   d5,d7
                asr.l   #8,d7
                add.w   a2,d7
                asl.w   d3,d7
                move.w  d7,(a5)+
                muls.w  (a1)+,d2
                muls.w  $2(a1),d4
                add.l   d2,d4
                asr.l   #8,d4
                add.w   a4,d4
                asl.w   d3,d4
                move.w  d4,(a5)+
                subq.w  #1,d1
                bgt.b   .next_pair
                movem.l d0/a3,-(a7)
                jsr     $C246A0.l
                movem.l (a7)+,d0/a3
                bra.w   $C2AF46
.packet_rejected:
                movem.l (a7)+,a0/a4
                bra.w   $C2AD00
