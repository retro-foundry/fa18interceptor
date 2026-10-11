; Byte-exact observed negated scaled-record delta stores $C14BB0-$C14BE7.
; Three prepared local longword deltas are negated into active-record fields
; +$3E, +$42 and +$46, then the shared event byte selects the continuation.

                org     $C14BB0

CURRENT_CONTROL_RECORD          equ     $C18210
POST_TICK_EVENT_FLAG             equ     $C457AE

store_negated_scaled_record_deltas:
                move.l  -$10(a6),d0
                neg.l   d0
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.l  d0,$3E(a0)
                move.l  -$14(a6),d0
                neg.l   d0
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.l  d0,$42(a0)
                move.l  -$18(a6),d0
                neg.l   d0
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.l  d0,$46(a0)
                tst.b   POST_TICK_EVENT_FLAG.l
                beq.b   $C14BF2
