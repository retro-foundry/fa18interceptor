; Byte-exact postflight label/template selection $C3238A-$C32463.
; Literal labels and copy destinations are proven; record-code semantics are not.

                org     $C3238A

POSTFLIGHT_PRIMARY_TEXT         equ     $C4580A
POSTFLIGHT_SECONDARY_TEXT       equ     $C4580E
POSTFLIGHT_TEXT_STYLE           equ     $C45862
POSTFLIGHT_RECORD_BASE          equ     $C46184
POSTFLIGHT_BLANK_TEXT           equ     $C326B8
POSTFLIGHT_NO_SIGNAL_TEXT       equ     $C32705
POSTFLIGHT_ALT_TEXT             equ     $C326D3
POSTFLIGHT_HDG_TEXT             equ     $C326D8
POSTFLIGHT_SPD_TEXT             equ     $C326DD
POSTFLIGHT_MIG29_TEXT           equ     $C326E2
POSTFLIGHT_MIG25_TEXT           equ     $C326E9
POSTFLIGHT_AF1_TEXT             equ     $C326F0
POSTFLIGHT_F16_TEXT             equ     $C326F7
POSTFLIGHT_CRUISE_TEXT          equ     $C326FE
POSTFLIGHT_707_TEXT             equ     $C3270C
POSTFLIGHT_767_TEXT             equ     $C32713

select_postflight_message_labels:
                lea.l   POSTFLIGHT_PRIMARY_TEXT.l,a2
                lea.l   POSTFLIGHT_BLANK_TEXT.l,a1
.copy_primary:
                move.b  (a1)+,d1
                beq.b   .primary_done
                move.b  d1,(a2)+
                bra.b   .copy_primary
.primary_done:
                andi.b  #$3F,POSTFLIGHT_TEXT_STYLE.l
                ori.b   #$80,POSTFLIGHT_TEXT_STYLE.l
                lea.l   POSTFLIGHT_RECORD_BASE.l,a4
                adda.w  d2,a4
                btst.b  #6,$20(a4)
                bne.b   .select_code_label
                lea.l   POSTFLIGHT_NO_SIGNAL_TEXT.l,a1
                andi.b  #$3F,POSTFLIGHT_TEXT_STYLE.l
                bra.w   .copy_secondary
.select_code_label:
                move.b  $62(a4),d1
                cmpi.b  #$12,d1
                bne.b   .check_code_13
                lea.l   POSTFLIGHT_MIG29_TEXT.l,a1
                bra.w   .copy_secondary
.check_code_13:
                cmpi.b  #$13,d1
                bne.b   .check_code_14
                lea.l   POSTFLIGHT_MIG25_TEXT.l,a1
                bra.b   .copy_secondary
.check_code_14:
                cmpi.b  #$14,d1
                bne.b   .check_code_16
                lea.l   POSTFLIGHT_AF1_TEXT.l,a1
                bra.b   .label_with_style_40
.check_code_16:
                cmpi.b  #$16,d1
                bne.b   .check_code_17
                lea.l   POSTFLIGHT_707_TEXT.l,a1
                bra.b   .label_with_style_40
.check_code_17:
                cmpi.b  #$17,d1
                bne.b   .check_code_10
                lea.l   POSTFLIGHT_767_TEXT.l,a1
                bra.b   .label_with_style_40
.check_code_10:
                cmpi.b  #$10,d1
                bne.b   .check_code_15
                lea.l   POSTFLIGHT_F16_TEXT.l,a1
.label_with_style_40:
                andi.b  #$3F,POSTFLIGHT_TEXT_STYLE.l
                ori.b   #$40,POSTFLIGHT_TEXT_STYLE.l
                bra.b   .copy_secondary
.check_code_15:
                cmpi.b  #$15,d1
                bne.b   .after_label_selection
                lea.l   POSTFLIGHT_CRUISE_TEXT.l,a1
                andi.b  #$3F,POSTFLIGHT_TEXT_STYLE.l
                ori.b   #$80,POSTFLIGHT_TEXT_STYLE.l
.copy_secondary:
                lea.l   POSTFLIGHT_SECONDARY_TEXT.l,a2
.copy_secondary_byte:
                move.b  (a1)+,d1
                beq.b   .after_label_selection
                move.b  d1,(a2)+
                bra.b   .copy_secondary_byte
.after_label_selection:
