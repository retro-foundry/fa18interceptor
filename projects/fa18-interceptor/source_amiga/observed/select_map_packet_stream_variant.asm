; Byte-exact static map-packet header and stream-variant selection $C2AEFC-$C2AF91.
; D7 arrives from the detail/culling preparation at $C2AE5A.

                org     $C2AEFC

select_map_packet_stream_variant:
                movem.l a0/a4,-(a7)
                move.l  (a3)+,d5
                blt.w   $C2AFF0
                move.l  $4(a4),d2
                tst.w   -$3e(a6)
                beq.b   .packet_origin_ready
                addi.l  #$1000,d2
                swap    d2
                rol.l   #4,d2
                bra.b   .project_origin
.packet_origin_ready:
                swap    d2
.project_origin:
                neg.w   d2
                lea     $C45BDA.l,a0
                move.w  d2,d1
                move.w  d2,d3
                muls.w  (a0),d1
                asr.l   #8,d1
                muls.w  $6(a0),d2
                asr.l   #8,d2
                muls.w  $c(a0),d3
                asr.l   #8,d3
                movem.w d1-d3,-$8(a6)
                tst.w   d7
                beq.b   .read_stream_word
                movea.l d5,a3
.read_stream_word:
                move.w  (a3)+,d1
                bgt.b   .count_ready
                cmpi.w  #$ffff,d1
                beq.w   $C2AFF0
                bclr    #$f,d1
                add.w   d1,d1
                add.w   d1,d1
                ext.l   d1
                cmp.l   -$28(a6),d1
                bgt.w   $C2AFF0
                move.w  (a3)+,d1
.count_ready:
                cmpi.w  #$12,d1
                ble.b   .count_accepted
                move.w  #$20,$C4599E.l
                jsr     $C06C02.l
                bra.w   $C2AFF0
.count_accepted:
                lea     $C4BF92.l,a5
                move.w  d1,(a5)+
                lea     $C45BD8.l,a0
                movem.w -$8(a6),d6/a2/a4
