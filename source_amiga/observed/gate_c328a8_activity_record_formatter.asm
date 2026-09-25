; Byte-exact activity-gated record formatter prefix $C328A8-$C328D9.
; The record-field and label semantics remain unresolved.

                org     $C328A8

ACTIVITY_FORMATTER_COUNT        equ     $C45844
DISPLAY_RECORD_BASE             equ     $C46184
DISPLAY_RECORD_OFFSET           equ     $C458DE
DISPLAY_RECORD_FORMAT_SCRATCH   equ     $C457FA
ACTIVITY_FORMATTER_TYPE_10      equ     $C329DA
ACTIVITY_FORMATTER_RETURN       equ     $C328A6

gate_c328a8_activity_record_formatter:
                tst.b   ACTIVITY_FORMATTER_COUNT.l
                ble.b   ACTIVITY_FORMATTER_RETURN
                subq.b  #1,ACTIVITY_FORMATTER_COUNT.l
                lea.l   DISPLAY_RECORD_BASE.l,a1
                adda.w  DISPLAY_RECORD_OFFSET.l,a1
                moveq   #0,d0
                move.b  $5F(a1),d0
                lea.l   DISPLAY_RECORD_FORMAT_SCRATCH.l,a2
                move.b  $63(a1),d2
                andi.b  #$F0,d2
                beq.w   ACTIVITY_FORMATTER_TYPE_10

