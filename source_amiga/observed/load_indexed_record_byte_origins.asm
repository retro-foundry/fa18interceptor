; Byte-exact observed indexed-record byte-origin load $C1EC58-$C1EC83.
; It resolves the record selected by d1's high byte and copies low-byte fields
; +$6/+8 into the local origins consumed by the following delta accumulators.

                org     $C1EC58

INDEXED_RECORD_BANK              equ     $C46184

load_indexed_record_byte_origins:
                move.l  a2,-(a7)
                andi.w  #$FF00,d1
                add.w   d1,d1
                lea     INDEXED_RECORD_BANK.l,a2
                adda.w  d1,a2
                move.w  $6(a2),d1
                andi.w  #$00FF,d1
                move.w  d1,-$1C(a6)
                move.w  $8(a2),d1
                andi.w  #$00FF,d1
                move.w  d1,-$1E(a6)
                movea.l (a7)+,a2
                rts
