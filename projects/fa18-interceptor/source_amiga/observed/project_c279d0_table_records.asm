; Byte-exact observed table-record projection loop $C27AF4-$C27C4D.
; Alternate branch targets inside this range are retained as raw addresses.

                org     $C27AF4

MATRIX_COEFFICIENTS             equ     $C45BD8
PROJECTED_PAIR_BUFFER           equ     $C4B392
PROJECTED_PAIR_BUFFER_END       equ     $C4B39E
PROJECTION_BOUNDS_TABLE         equ     $C27D24
POLYGON_SUBMISSION              equ     $C2FF48

project_c279d0_table_records:
                lea     $C45BDA.l,a0
                move.w  -$4(a6),d2
                move.w  d2,d1
                move.w  d2,d3
                muls.w  (a0),d1
                asr.l   #8,d1
                muls.w  $6(a0),d2
                asr.l   #8,d2
                muls.w  $c(a0),d3
                asr.l   #8,d3
                movem.w d1-d3,-$20(a6)
                move.w  #3,$C4B390.l
.record:
                movem.w (a3)+,d0-d2
                move.w  d2,-$18(a6)
                add.w   -$6(a6),d0
                add.w   -$2(a6),d1
                move.w  d0,d3
                bge.b   .abs_x_ready
                neg.w   d3
.abs_x_ready:
                move.w  d1,d4
                bge.b   .abs_y_ready
                neg.w   d4
.abs_y_ready:
                asr.w   #8,d3
                asr.w   #8,d4
                lsl.w   #5,d4
                add.w   d3,d4
                lea     PROJECTION_BOUNDS_TABLE.l,a4
                move.b  $0(a4,d4.w),d3
                ext.w   d3
                cmp.w   -$1a(a6),d3
                bgt.w   $C27C3C
                tst.w   d2
                bge.b   .record_kind_ready
                tst.w   -$22(a6)
                bne.b   .negative_kind
                subq.w  #1,d3
                ble.b   .record_kind_ready
.negative_kind:
                cmpi.w  #-$c,d2
                bne.b   .kind_two
                move.w  #1,d2
                bra.b   .store_kind
.kind_two:
                move.w  #2,d2
.store_kind:
                move.w  d2,-$18(a6)
.record_kind_ready:
                move.w  -$16(a6),d3
                asl.w   d3,d0
                asl.w   d3,d1
                movea.w d0,a4
                movea.w d1,a5
                movem.w -$20(a6),d1/d5/d7
                tst.w   d2
                bge.w   $C27C62
                lea     PROJECTED_PAIR_BUFFER.l,a1
.record_source:
                movea.l $0(a6,d2.w),a2
.pair:
                movem.w (a2)+,d3-d4
                add.w   a4,d3
                add.w   a5,d4
                lea     MATRIX_COEFFICIENTS.l,a0
                move.w  (a0)+,d0
                addq.w  #2,a0
                move.w  (a0)+,d2
                muls.w  d3,d0
                muls.w  d4,d2
                add.l   d2,d0
                asr.l   #8,d0
                add.w   d1,d0
                move.w  d0,d6
                move.w  (a0)+,d0
                addq.w  #2,a0
                move.w  (a0)+,d2
                muls.w  d3,d0
                muls.w  d4,d2
                add.l   d0,d2
                asr.l   #8,d2
                add.w   d5,d2
                muls.w  (a0)+,d3
                muls.w  $2(a0),d4
                add.l   d3,d4
                asr.l   #8,d4
                add.w   d7,d4
                ble.w   $C27C3C
                cmp.w   d4,d6
                bgt.b   $C27C3C
                move.w  d6,d0
                neg.w   d0
                cmp.w   d4,d0
                bgt.b   $C27C3C
                cmp.w   d4,d2
                bgt.b   $C27C3C
                move.w  d2,d0
                neg.w   d0
                cmp.w   d4,d0
                bgt.b   $C27C3C
                muls.w  #$a0,d6
                divs.w  d4,d6
                addi.w  #$a0,d6
                blt.b   $C27C4E
                cmpi.w  #$140,d6
                bge.b   $C27C56
                muls.w  #$5a,d2
                divs.w  d4,d2
                addi.w  #$5a,d2
                blt.b   $C27C52
                cmpi.w  #$b4,d2
                bge.b   $C27C5C
                subi.w  #$13f,d6
                neg.w   d6
                subi.w  #$b3,d2
                neg.w   d2
                move.w  d6,(a1)+
                move.w  d2,(a1)+
                cmpa.l  #PROJECTED_PAIR_BUFFER_END,a1
                blt.w   .pair
                move.l  a3,-(sp)
                jsr     POLYGON_SUBMISSION.l
                movea.l (sp)+,a3
                subq.w  #1,-$8(a6)
                bgt.w   .record
                clr.b   $C457A2.l
                unlk    a6
                rts
