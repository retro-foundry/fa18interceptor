; Byte-exact observed indexed-record byte-delta accumulation $C1EC96-$C1ECD3.
; The high byte of the a1 control word selects a word-aligned record-bank
; entry.  Two unsigned low-byte fields are offset by frame-local words,
; promoted to fixed-point longs, divided by four, and accumulated into d2/d4.

                org     $C1EC96

INDEXED_DELTA_RECORD_BASE       equ     $C46184

accumulate_indexed_record_byte_deltas:
                move.l  a2,-(a7)
                move.w  (a1),d1
                andi.w  #$FF00,d1
                add.w   d1,d1
                lea     INDEXED_DELTA_RECORD_BASE.l,a2
                adda.w  d1,a2
                moveq   #0,d1
                move.w  $6(a2),d1
                andi.w  #$00FF,d1
                sub.w   -$1C(a6),d1
                swap    d1
                asr.l   #2,d1
                add.l   d1,d2
                moveq   #0,d1
                move.w  $8(a2),d1
                andi.w  #$00FF,d1
                sub.w   -$1E(a6),d1
                swap    d1
                asr.l   #2,d1
                add.l   d1,d4
                movea.l (a7)+,a2
                rts
