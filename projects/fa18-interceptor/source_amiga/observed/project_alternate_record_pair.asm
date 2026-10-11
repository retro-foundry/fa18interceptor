; Byte-exact observed alternate record-pair projection $C1F34A-$C1F3DD.
; It gates a pair against the shared depth component, derives three offsets from
; A4, and writes a projected triple to A3. Physical object identity is unknown.

                org     $C1F34A

SHARED_DEPTH_COMPONENT          equ     $C45A78
LIVE_SHIFT_COUNT                equ     $C45AB8
ALTERNATE_LOOP_CONTINUE         equ     $C1F344
POST_PROJECT_GUARD              equ     $C1F404

project_alternate_record_pair:
                cmpi.l  #-$20,SHARED_DEPTH_COMPONENT.l
                blt.s   .project_pair
                andi.w  #4,d7
                beq.s   .small_limit
                move.w  #$170,d7
                bra.s   .apply_limit_shift
.small_limit:
                move.w  #$60,d7
.apply_limit_shift:
                move.w  LIVE_SHIFT_COUNT.l,d6
                asr.w   d6,d7
                cmp.w   -$28(a6),d7
                blt.s   ALTERNATE_LOOP_CONTINUE
.project_pair:
                move.w  a5,d3
                move.w  d3,d6
                muls.w  $e(a4),d3
                asr.l   #8,d3
                movea.w d3,a5
                move.w  d6,d3
                muls.w  2(a4),d3
                asr.l   #8,d3
                muls.w  8(a4),d6
                asr.l   #8,d6
                movem.w d3/d6/a5,-$78(a6)
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
                tst.w   -$62(a6)
                beq.s   POST_PROJECT_GUARD
