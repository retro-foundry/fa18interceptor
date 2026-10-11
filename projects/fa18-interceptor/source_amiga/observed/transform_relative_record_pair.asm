; Byte-exact observed relative-record transform tail $C1E95C-$C1EA49.
; The code derives two signed three-component differences, forms cross-product
; components, then evaluates two scaled dot products. Record identities are not
; established by this reconstruction.

                org     $C1E95C

RECORD_BANK_BASE                equ     $C46184
SHARED_DEPTH_COMPONENT          equ     $C45A78
TRANSFORM_CONTINUE              equ     $C1EA4E

transform_relative_record_pair:
                movea.l (sp)+,a4
.next_record:
                addq.w  #4,a4
.record_pointer:
                movea.l (a4)+,a2
                movem.w 2(a2),d2-d4
                andi.w  #$0FFF,d4
                addi.w  #$A4,d2
                addi.w  #$A4,d3
                addi.w  #$A4,d4
                move.w  (a0),d1
                andi.w  #$FF00,d1
                add.w   d1,d1
                lea     RECORD_BANK_BASE.l,a2
                adda.w  d1,a2
                lea     (a2,d2.w),a5
                move.w  d4,d1
                movem.w (a2,d3.w),d2-d4
                sub.w   (a5),d2
                sub.w   2(a5),d3
                sub.w   4(a5),d4
                movem.w (a2,d1.w),d5-d7
                sub.w   (a5),d5
                sub.w   2(a5),d6
                sub.w   4(a5),d7
                move.w  d4,d0
                move.w  d7,d1
                muls.w  d3,d7
                muls.w  d6,d4
                sub.l   d4,d7
                asr.l   #6,d7
                muls.w  d5,d0
                muls.w  d2,d1
                sub.l   d1,d0
                asr.l   #6,d0
                muls.w  d2,d6
                muls.w  d3,d5
                sub.l   d5,d6
                asr.l   #6,d6
                move.w  d7,d5
                move.w  d6,d7
                move.w  d0,d6
                movem.w (a5),d2-d4
                move.w  -$20(a6),d0
                asr.w   d0,d2
                asr.l   d0,d3
                asr.w   d0,d4
                add.w   $c(a2),d2
                add.l   $10(a2),d3
                add.w   $e(a2),d4
                sub.w   -2(a6),d2
                sub.l   -6(a6),d3
                sub.w   -8(a6),d4
                neg.w   d2
                neg.l   d3
                neg.w   d4
                muls.w  d5,d2
                muls.w  d6,d3
                muls.w  d7,d4
                add.l   d3,d4
                add.l   d2,d4
                move.l  d4,d1
                movem.w (a5),d2-d4
                asr.w   d0,d2
                asr.l   d0,d3
                asr.w   d0,d4
                add.w   $c(a2),d2
                add.l   $10(a2),d3
                add.w   $e(a2),d4
                ext.l   d2
                ext.l   d4
                sub.l   -$c(a6),d2
                add.l   SHARED_DEPTH_COMPONENT.l,d3
                sub.l   -$10(a6),d4
                neg.l   d2
                neg.l   d3
                neg.l   d4
                muls.w  d5,d2
                muls.w  d6,d3
                muls.w  d7,d4
                add.l   d3,d4
                add.l   d2,d4
                bge.s   TRANSFORM_CONTINUE
                subq.w  #1,-$12(a6)
                bge.w   .record_pointer
