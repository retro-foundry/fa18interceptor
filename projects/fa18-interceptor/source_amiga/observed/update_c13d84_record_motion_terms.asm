; Byte-exact observed C13D84 continuation $C142B6-$C14317.
; It shifts a local term into record +$72 and prepares subsequent signed local
; terms under a record-flag gate; physical field meaning remains open.

                org     $C142B6

CURRENT_CONTROL_RECORD          equ     $C18210
RECORD_OFFSET_LONG_72           equ     $72

update_c13d84_record_motion_terms:
                moveq   #9,d0
                move.w  -$26(a6),d1
                asr.w   d0,d1
                move.w  d1,-$26(a6)
                ext.l   d1
                movea.l CURRENT_CONTROL_RECORD.l,a0
                sub.l   d1,RECORD_OFFSET_LONG_72(a0)
                tst.w   -$24(a6)
                bmi.w   $C14500
                move.w  -$18(a6),d0
                neg.w   d0
                movea.l -$30(a6),a0
                move.w  (a0),d1
                move.w  d0,-$1c(a6)
                btst    #7,d1
                bne.w   $C14410
                move.w  -$24(a6),d0
                asr.w   #1,d0
                move.w  -$24(a6),d1
                asr.w   #3,d1
                sub.w   d1,d0
                move.w  d0,-$16(a6)
                asr.w   #2,d0
                move.w  -$16(a6),d1
                sub.w   d0,d1
                movea.l -$14(a6),a0
                move.b  (a0),d0
                move.w  d1,-$16(a6)
                btst    #0,d0
                beq.b   $C1431E
