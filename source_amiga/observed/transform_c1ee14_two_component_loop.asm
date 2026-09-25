; Byte-exact observed C1EE14 transform branch $C1F404-$C1F463.
; It transforms counted input pairs through selected matrix lanes and writes
; three-word output records.  Stream ownership remains structural.

                org     $C1F404

ALT_TRANSFORM_BRANCH            equ     $C07846

transform_c1ee14_two_component_loop:
                tst.w   (a0)
                ble.w   $C1F6F8
                move.w  (a1)+,d2
                move.w  (a1)+,d4
                move.w  -$8(a6),d7
                asr.w   d7,d2
                asr.w   d7,d4
                add.w   d0,d2
                add.w   d1,d4
                lea     (a4),a2
                move.w  d2,d5
                move.w  d4,d7
                muls.w  (a2)+,d5
                addq.w  #2,a2
                muls.w  (a2)+,d7
                add.l   d5,d7
                asr.l   #8,d7
                add.w   d3,d7
                move.w  d7,(a3)+
                move.w  d2,d5
                move.w  d4,d7
                muls.w  (a2)+,d5
                addq.w  #2,a2
                muls.w  (a2)+,d7
                add.l   d5,d7
                asr.l   #8,d7
                add.w   d6,d7
                move.w  d7,(a3)+
                muls.w  (a2)+,d2
                muls.w  2(a2),d4
                add.l   d2,d4
                asr.l   #8,d4
                add.w   a5,d4
                move.w  d4,(a3)+
                subq.w  #1,(a0)
                bgt.b   $C1F40A
                move.b  -$64(a6),d4
                andi.b  #4,d4
                beq.w   $C1F6F8
                jmp     ALT_TRANSFORM_BRANCH.l
