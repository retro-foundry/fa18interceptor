; Byte-exact observed indexed transform-record field loader $C1EBC0-$C1EBDF.
; The high byte of D1 chooses a 512-byte record in $C46184; it returns the
; signed +$0C and +$0E fields in D2/D4 and the +$10 long in D3.

                org     $C1EBC0

INDEXED_TRANSFORM_RECORD_BASE    equ     $C46184

load_indexed_transform_record_fields:
                andi.w  #$FF00,d1
                add.w   d1,d1
                lea     INDEXED_TRANSFORM_RECORD_BASE.l,a2
                adda.w  d1,a2
                move.w  $c(a2),d2
                ext.l   d2
                move.l  $10(a2),d3
                move.w  $e(a2),d4
                ext.l   d4
                rts
