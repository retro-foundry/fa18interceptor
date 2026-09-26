; Byte-exact observed indexed-record transform setup $C1E766-$C1E799.
; It derives a record-bank offset from d1's high byte, loads two component
; words and one longword, accumulates the matching byte deltas, then branches
; on the selected record's top two flag bits.

                org     $C1E766

INDEXED_RECORD_BANK              equ     $C46184
ACCUMULATE_INDEXED_BYTE_DELTAS   equ     $C1EC96

prepare_indexed_record_transform_components:
                lea     INDEXED_RECORD_BANK.l,a3
                andi.w  #$FF00,d1
                add.w   d1,d1
                move.w  $C(a3,d1.w),d2
                move.w  $E(a3,d1.w),d4
                move.l  $10(a3,d1.w),-$6(a6)
                move.w  d1,-(a7)
                bsr.w   ACCUMULATE_INDEXED_BYTE_DELTAS
                move.w  (a7)+,d1
                move.w  d2,-$2(a6)
                move.w  d4,-$8(a6)
                move.b  $4(a3,d1.w),d0
                andi.b  #$C0,d0
                beq.b   $C1E7E6
