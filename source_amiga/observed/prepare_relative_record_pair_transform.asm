; Byte-exact observed relative-record transform setup $C1E872-$C1E8C7.
; It resolves a record-bank subentry, applies a shared depth threshold, and
; initializes the record cursor consumed by the C1E8D6 transform continuation.

                org     $C1E872

RECORD_BANK_BASE                equ     $C46184
SHARED_DEPTH_COMPONENT          equ     $C45A78

prepare_relative_record_pair_transform:
                andi.w  #$3FFF,d0
                move.w  d0,-$12(a6)
                move.l  (a4)+,-$16(a6)
                move.l  a4,-(sp)
                move.w  (a0),d1
                andi.w  #$FF00,d1
                add.w   d1,d1
                lea     RECORD_BANK_BASE.l,a2
                adda.w  d1,a2
                movea.l (a4)+,a4
                move.w  (a4),d0
                addi.w  #$A4,d0
                move.l  SHARED_DEPTH_COMPONENT.l,d1
                neg.l   d1
                move.b  $7d(a2),d3
                andi.w  #$F,d3
                move.w  d3,-$20(a6)
                move.w  2(a2,d0.w),d0
                asr.w   d3,d0
                cmp.w   d0,d1
                blt.w   $C1E95C
                clr.w   -$18(a6)
                move.w  (a4),-$1a(a6)
                blt.w   $C1E95C
                move.w  (a4)+,d0
                bge.b   $C1E8D6
