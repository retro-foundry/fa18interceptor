; Byte-exact observed continuation in the $C13D84 control-record update,
; $C14D0C-$C14D93.  The record-field role is not yet established.

                org     $C14D0C

CURRENT_CONTROL_RECORD          equ     $C18210
RECORD_OFFSET_LONG_18           equ     $18
RECORD_OFFSET_BYTE_62           equ     $62
RECORD_OFFSET_WORD_4C           equ     $4C

update_c13d84_record_offset18:
                move.l  #$708,-$20(a6)
                bra.b   $C14D1E
                move.l  #$2EE,-$20(a6)
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.l  RECORD_OFFSET_LONG_18(a0),d0
                sub.l   -$14(a6),d0
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.l  d0,RECORD_OFFSET_LONG_18(a0)
                move.l  -$20(a6),d1
                cmp.l   d1,d0
                bgt.w   $C1505E
                movea.l -$34(a6),a0
                move.b  (a0),d0
                andi.b  #$FB,d0
                move.b  d0,(a0)
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.b  RECORD_OFFSET_BYTE_62(a0),d0
                andi.b  #$F0,d0
                tst.b   d0
                beq.b   $C14D68
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.l  d1,RECORD_OFFSET_LONG_18(a0)
                movea.l -$30(a6),a0
                move.b  (a0),d0
                andi.b  #$C0,d0
                tst.b   d0
                bne.b   $C14DE0
                movea.l -$2c(a6),a0
                move.w  (a0),d0
                btst    #7,d0
                beq.b   $C14D94
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.w  RECORD_OFFSET_WORD_4C(a0),d0
                andi.w  #7,d0
                tst.w   d0
                bne.b   $C14DE0
