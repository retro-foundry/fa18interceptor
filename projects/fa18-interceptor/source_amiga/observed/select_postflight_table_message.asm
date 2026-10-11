; Byte-exact postflight table-message fallback $C32510-$C325A5.
; The 28-byte table-record semantics remain unresolved.

                org     $C32510

POSTFLIGHT_SLOT_WORD            equ     $C459C4
POSTFLIGHT_MESSAGE_SELECTOR     equ     $C45ADE
POSTFLIGHT_PREVIOUS_SELECTOR    equ     $C45AE4
POSTFLIGHT_MESSAGE_TABLE        equ     $C3D0A0
POSTFLIGHT_PRIMARY_TEXT         equ     $C4580A
POSTFLIGHT_SLOT_DELAY           equ     $C4583C
POSTFLIGHT_TABLE_MESSAGE_SUBMIT equ     $C325A6
POSTFLIGHT_TABLE_MESSAGE_RETURN equ     $C32678

select_postflight_table_message:
                tst.w   POSTFLIGHT_SLOT_WORD.l
                beq.b   .update_table_message
                bgt.b   .set_negative_slot
                bra.b   .clear_slot
.update_table_message:
                move.w  POSTFLIGHT_MESSAGE_SELECTOR.l,d0
                cmp.w   POSTFLIGHT_PREVIOUS_SELECTOR.l,d0
                beq.b   .reuse_or_return
                move.w  d0,POSTFLIGHT_PREVIOUS_SELECTOR.l
                andi.w  #$FF,d0
                add.w   d0,d0
                add.w   d0,d0
                move.w  d0,d1
                add.w   d0,d0
                add.w   d0,d1
                add.w   d0,d0
                add.w   d0,d1
                addq.w  #1,d1
                lea.l   POSTFLIGHT_MESSAGE_TABLE.l,a1
                adda.w  d1,a1
                lea.l   POSTFLIGHT_PRIMARY_TEXT.l,a2
                moveq   #$1A,d1
.copy_table_message:
                move.b  (a1)+,(a2)+
                dbra    d1,.copy_table_message
                move.b  #2,POSTFLIGHT_SLOT_DELAY.l
                bra.b   POSTFLIGHT_TABLE_MESSAGE_SUBMIT
.set_negative_slot:
                move.w  #-$1,POSTFLIGHT_SLOT_WORD.l
                bra.b   .copy_first_table_message
.clear_slot:
                clr.w   POSTFLIGHT_SLOT_WORD.l
                bra.b   .copy_first_table_message
.set_table_message_delay:
                move.b  #2,POSTFLIGHT_SLOT_DELAY.l
.copy_first_table_message:
                lea.l   POSTFLIGHT_MESSAGE_TABLE.l,a1
                addq.w  #1,a1
                lea.l   POSTFLIGHT_PRIMARY_TEXT.l,a2
                moveq   #$1A,d1
.copy_first_table_message_loop:
                move.b  (a1)+,(a2)+
                dbra    d1,.copy_first_table_message_loop
                bra.b   POSTFLIGHT_TABLE_MESSAGE_SUBMIT
.reuse_or_return:
                tst.b   POSTFLIGHT_SLOT_DELAY.l
                ble.w   POSTFLIGHT_TABLE_MESSAGE_RETURN
                subq.b  #1,POSTFLIGHT_SLOT_DELAY.l
