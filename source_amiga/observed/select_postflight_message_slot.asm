; Byte-exact postflight message-slot control $C32318-$C32389.
; The selected message/template and state-field ownership remain unresolved.

                org     $C32318

POSTFLIGHT_SLOT_DELAY           equ     $C4583C
POSTFLIGHT_RESULT_BYTE          equ     $C45886
POSTFLIGHT_SLOT_WORD            equ     $C459C4
POSTFLIGHT_SLOT_SKIP            equ     $C322EC
POSTFLIGHT_SLOT_SUBMIT          equ     $C325A6
POSTFLIGHT_SLOT_RETURN          equ     $C32678

select_postflight_message_slot:
                lea.l   $C46184,a1
                tst.b   POSTFLIGHT_SLOT_DELAY.l
                bgt.b   .load_slot_word
                tst.b   POSTFLIGHT_RESULT_BYTE.l
                blt.b   .advance_negative_result
                btst.b  #0,POSTFLIGHT_RESULT_BYTE.l
                beq.b   POSTFLIGHT_SLOT_SKIP
                move.w  POSTFLIGHT_SLOT_WORD.l,d0
                bra.b   .select_slot
.advance_negative_result:
                move.b  #1,POSTFLIGHT_RESULT_BYTE.l
                move.w  POSTFLIGHT_SLOT_WORD.l,d0
                addq.w  #1,d0
                cmpi.w  #3,d0
                ble.b   .select_slot
                moveq   #1,d0
.select_slot:
                bset    #15,d0
                move.w  d0,POSTFLIGHT_SLOT_WORD.l
                move.b  #2,POSTFLIGHT_SLOT_DELAY.l
.load_slot_word:
                move.w  POSTFLIGHT_SLOT_WORD.l,d0
                bclr    #15,d0
                bne.b   .slot_changed
                subq.b  #1,POSTFLIGHT_SLOT_DELAY.l
                bge.w   POSTFLIGHT_SLOT_SUBMIT
                bra.w   POSTFLIGHT_SLOT_RETURN
.slot_changed:
                move.w  d0,POSTFLIGHT_SLOT_WORD.l
