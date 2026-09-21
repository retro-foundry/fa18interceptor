; Byte-exact static-only divided-record setup $C337FC-$C338AA.
                org     $C337FC
POSTFLIGHT_RECORD_BASE          equ $C46184
POSTFLIGHT_RECORD_OFFSET        equ $C458DE
POSTFLIGHT_VALUE_STORE          equ $C45B1E
POSTFLIGHT_PACKED_VALUE         equ $C45B22
POSTFLIGHT_RESULT_WORD          equ $C4598C
POSTFLIGHT_TABLE_BASE           equ $C33A16
POSTFLIGHT_HELPER_A             equ $C25A08
POSTFLIGHT_HELPER_B             equ $C259C2
POSTFLIGHT_SCALED_HELPER        equ $C33F54
prepare_postflight_divided_record_value:
                lea.l   POSTFLIGHT_RECORD_BASE.l,a0
                adda.w  POSTFLIGHT_RECORD_OFFSET.l,a0
                move.w  $68(a0),d0
                asr.w   #3,d0
                ext.l   d0
                moveq   #0,d2
                move.l  d0,POSTFLIGHT_VALUE_STORE.l
                jsr     POSTFLIGHT_HELPER_A.l
                move.l  POSTFLIGHT_PACKED_VALUE.l,d0
                move.l  d0,d1
                andi.w  #$FF0F,d0
                andi.w  #$FF,d1
                cmpi.w  #$40,d1
                ble.s   postflight_divided_packed_ready
                dc.w    $243C
                dc.l    -$64
                addi.l  #$64,POSTFLIGHT_VALUE_STORE.l
                jsr     POSTFLIGHT_HELPER_A.l
                move.l  POSTFLIGHT_PACKED_VALUE.l,d0
                andi.w  #$FF00,d0
postflight_divided_packed_ready:
                lsr.l   #4,d0
                cmpi.l  #$360,d0
                blt.s   postflight_divided_packed_in_range
                moveq   #0,d0
postflight_divided_packed_in_range:
                move.l  d0,POSTFLIGHT_PACKED_VALUE.l
                move.l  POSTFLIGHT_PACKED_VALUE.l,-(a7)
                move.l  d1,POSTFLIGHT_PACKED_VALUE.l
                jsr     POSTFLIGHT_HELPER_B.l
                move.l  (a7)+,POSTFLIGHT_PACKED_VALUE.l
                move.l  POSTFLIGHT_VALUE_STORE.l,d1
                add.l   d2,d1
                divs.w  #5,d1
                neg.w   d1
                move.w  d1,d5
                addi.w  #$9F,d5
                move.w  d5,POSTFLIGHT_RESULT_WORD.l
                asl.w   #3,d1
                lea.l   POSTFLIGHT_TABLE_BASE.l,a1
                adda.w  d1,a1
                move.w  d1,-(a7)
                bsr.w   POSTFLIGHT_SCALED_HELPER
                move.w  (a7)+,d1
