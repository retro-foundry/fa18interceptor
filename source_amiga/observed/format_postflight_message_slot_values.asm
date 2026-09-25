; Byte-exact postflight slot-value setup $C32464-$C3250F.
; The literals and arithmetic are proven; field and UI ownership remain unresolved.

                org     $C32464

POSTFLIGHT_LABEL_TEXT           equ     $C45817
POSTFLIGHT_VALUE_TEXT           equ     $C45820
POSTFLIGHT_ALT_VALUE_TEXT       equ     $C45821
POSTFLIGHT_ALT_LABEL            equ     $C326D3
POSTFLIGHT_HDG_LABEL            equ     $C326D8
POSTFLIGHT_SPD_LABEL            equ     $C326DD
POSTFLIGHT_VALUE_FORMATTER      equ     $C3267A
POSTFLIGHT_SLOT_SUBMIT          equ     $C325A6

format_postflight_message_slot_values:
                lea.l   POSTFLIGHT_LABEL_TEXT.l,a2
                subq.w  #1,d0
                bne.b   .slot_two
                lea.l   POSTFLIGHT_ALT_LABEL.l,a1
.copy_alt_label:
                move.b  (a1)+,d1
                beq.b   .alt_label_done
                move.b  d1,(a2)+
                bra.b   .copy_alt_label
.alt_label_done:
                move.l  $18(a4),d0
                asr.l   #7,d0
                asr.l   #3,d0
                move.l  d0,d1
                add.l   d0,d0
                add.l   d0,d0
                add.l   d1,d0
                lea.l   POSTFLIGHT_ALT_VALUE_TEXT.l,a2
                moveq   #4,d2
                moveq   #0,d4
                bsr.w   POSTFLIGHT_VALUE_FORMATTER
                bra.w   POSTFLIGHT_SLOT_SUBMIT
.slot_two:
                subq.w  #1,d0
                bne.b   .slot_three
                lea.l   POSTFLIGHT_HDG_LABEL.l,a1
.copy_hdg_label:
                move.b  (a1)+,d1
                beq.b   .hdg_label_done
                move.b  d1,(a2)+
                bra.b   .copy_hdg_label
.hdg_label_done:
                move.w  $68(a4),d0
                ext.l   d0
                asr.l   #3,d0
                divu.w  #$A,d0
                ext.l   d0
                lea.l   POSTFLIGHT_VALUE_TEXT.l,a2
                moveq   #2,d2
                moveq   #1,d4
                bsr.w   POSTFLIGHT_VALUE_FORMATTER
                bra.w   POSTFLIGHT_SLOT_SUBMIT
.slot_three:
                subq.w  #1,d0
                bne.w   POSTFLIGHT_SLOT_SUBMIT
                lea.l   POSTFLIGHT_SPD_LABEL.l,a1
.copy_spd_label:
                move.b  (a1)+,d1
                beq.b   .spd_label_done
                move.b  d1,(a2)+
                bra.b   .copy_spd_label
.spd_label_done:
                moveq   #0,d0
                ; Preserve the original explicit zero-displacement encoding.
                dc.w    $082C,$0007,$0000
                bne.b   .speed_magnitude_ready
                move.w  $6E(a4),d0
                bge.b   .speed_magnitude_ready
                neg.w   d0
.speed_magnitude_ready:
                ext.l   d0
                divu.w  #$C,d0
                ext.l   d0
                lea.l   POSTFLIGHT_VALUE_TEXT.l,a2
                moveq   #3,d2
                moveq   #0,d4
                bsr.w   POSTFLIGHT_VALUE_FORMATTER
                bra.w   POSTFLIGHT_SLOT_SUBMIT
