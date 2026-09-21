; Byte-exact static-only postflight scaled-value setup $C33370-$C333B1.

                org     $C33370

POSTFLIGHT_RENDERER_SELECTOR     equ $C45954
POSTFLIGHT_RECORD_BASE           equ $C46184
POSTFLIGHT_RECORD_OFFSET          equ $C458DE
POSTFLIGHT_SOURCE_FLAG           equ $C457A4
POSTFLIGHT_SOURCE_LONG           equ $C45658

prepare_postflight_scaled_value:
                move.w  #$D,POSTFLIGHT_RENDERER_SELECTOR.l
                lea     POSTFLIGHT_RECORD_BASE.l,a0
                adda.w  POSTFLIGHT_RECORD_OFFSET.l,a0
                tst.b   POSTFLIGHT_SOURCE_FLAG.l
                beq.s   postflight_scaled_from_record
                move.l  POSTFLIGHT_SOURCE_LONG.l,d0
                cmpi.l  #$1869F,d0
                ble.s   postflight_scaled_ready
                move.l  #$1869F,d0
                bra.s   postflight_scaled_ready
postflight_scaled_from_record:
                move.l  $18(a0),d0
                asr.l   #7,d0
                asr.l   #3,d0
                move.l  d0,d1
                add.l   d0,d0
                add.l   d0,d0
                add.l   d1,d0
postflight_scaled_ready:
