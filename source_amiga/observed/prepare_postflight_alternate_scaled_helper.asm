; Byte-exact static-only alternate scaled-helper setup $C33642-$C336F9.
                org     $C33642
postflight_helper_input_word:
                dc.w    $4B54
POSTFLIGHT_VALUE_STORE          equ $C45B1E
POSTFLIGHT_PACKED_VALUE         equ $C45B22
POSTFLIGHT_STATUS_LONG          equ $C45B26
POSTFLIGHT_VERTICAL_OFFSET      equ $C458D8
POSTFLIGHT_RESULT_WORD          equ $C4598C
POSTFLIGHT_HELPER_A             equ $C25A08
POSTFLIGHT_HELPER_B             equ $C259C2
POSTFLIGHT_SCALED_HELPER        equ $C33F70
prepare_postflight_alternate_scaled_helper:
                ext.l   d0
                divu.w  #$C,d0
                ext.l   d0
                moveq   #0,d2
                move.l  d0,POSTFLIGHT_VALUE_STORE.l
                jsr     POSTFLIGHT_HELPER_A.l
                move.l  POSTFLIGHT_PACKED_VALUE.l,d0
                move.l  d0,d1
                andi.w  #$FF0F,d0
                andi.w  #$FF,d1
                cmpi.w  #$20,d1
                ble.s   postflight_alternate_packed_ready
                cmpi.w  #$70,d1
                bgt.s   postflight_alternate_high_packed
                ori.w   #$50,d0
                dc.w    $243C
                dc.l    -$32
                bra.s   postflight_alternate_packed_ready
postflight_alternate_high_packed:
                dc.w    $243C
                dc.l    -$64
                addi.l  #$64,POSTFLIGHT_VALUE_STORE.l
                jsr     POSTFLIGHT_HELPER_A.l
                move.l  POSTFLIGHT_PACKED_VALUE.l,d0
                andi.w  #$FF00,d0
postflight_alternate_packed_ready:
                lsr.l   #4,d0
                move.l  d0,POSTFLIGHT_PACKED_VALUE.l
                move.l  POSTFLIGHT_PACKED_VALUE.l,-(a7)
                move.l  d1,POSTFLIGHT_PACKED_VALUE.l
                jsr     POSTFLIGHT_HELPER_B.l
                move.l  (a7)+,POSTFLIGHT_PACKED_VALUE.l
                move.l  POSTFLIGHT_VALUE_STORE.l,d1
                add.l   d2,d1
                divs.w  #3,d1
                move.w  d1,d5
                addi.w  #$5B,d1
                add.w   POSTFLIGHT_VERTICAL_OFFSET.l,d1
                move.w  d1,POSTFLIGHT_RESULT_WORD.l
                asl.w   #3,d5
                move.w  d5,d3
                add.w   d3,d3
                add.w   d3,d3
                add.w   d3,d5
                ext.l   d5
                addi.l  #$DF2,d5
                move.l  d5,-(a7)
                bsr.w   POSTFLIGHT_SCALED_HELPER
                move.l  (a7)+,d5
