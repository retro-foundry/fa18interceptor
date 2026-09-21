; Byte-exact static-only shared variant path $C33C18-$C33CC3.
                org     $C33C18
POSTFLIGHT_VARIANT_PAIR         equ $C4593E
POSTFLIGHT_VERTICAL_OFFSET      equ $C458D8
POSTFLIGHT_STATE_WORD_A         equ $C459C0
POSTFLIGHT_RECORD_BASE          equ $C46184
POSTFLIGHT_RECORD_OFFSET        equ $C458DE
POSTFLIGHT_SELECTOR             equ $C45954
POSTFLIGHT_FLAG_BYTE            equ $C457AE
POSTFLIGHT_WORD_A               equ $C45B44
POSTFLIGHT_WORD_B               equ $C45B46
POSTFLIGHT_HELPER_A             equ $C345A0
POSTFLIGHT_HELPER_B             equ $C347F2
POSTFLIGHT_HELPER_C             equ $C31D16
run_postflight_shared_variant_path:
                movem.w POSTFLIGHT_VARIANT_PAIR.l,d0-d1
                tst.w   d0
                ble.w   $C33CC4
                add.w   POSTFLIGHT_VERTICAL_OFFSET.l,d1
                move.w  #$50,d2
                lea.l   $C3494C.l,a0
                move.w  #$A,POSTFLIGHT_SELECTOR.l
                bsr.w   POSTFLIGHT_HELPER_A
                tst.w   POSTFLIGHT_STATE_WORD_A.l
                blt.w   $C33CC4
                movem.w POSTFLIGHT_VARIANT_PAIR.l,d0-d1
                add.w   POSTFLIGHT_VERTICAL_OFFSET.l,d1
                lea.l   POSTFLIGHT_RECORD_BASE.l,a0
                adda.w  POSTFLIGHT_RECORD_OFFSET.l,a0
                move.w  $4A(a0),d2
                move.l  a0,-(a7)
                mulu.w  #$4C,d2
                divu.w  #$8CA0,d2
                lea.l   $C34976.l,a0
                move.w  #$D,POSTFLIGHT_SELECTOR.l
                bsr.w   POSTFLIGHT_HELPER_B
                movea.l (a7)+,a0
                move.w  $4A(a0),d0
                cmpi.w  #$7F00,d0
                bgt.s   $C33CC4
                tst.b   POSTFLIGHT_FLAG_BYTE.l
                bne.s   postflight_shared_existing_flag
                dc.w    $08A8,$0000,$0004 ; bclr.b #0,$4(a0)
                bne.s   postflight_shared_update_words
postflight_shared_existing_flag:
                move.w  POSTFLIGHT_WORD_B.l,d0
                bra.s   postflight_shared_invoke_helper
postflight_shared_update_words:
                move.w  d0,d1
                sub.w   POSTFLIGHT_WORD_A.l,d0
                move.w  d0,POSTFLIGHT_WORD_B.l
                move.w  d1,POSTFLIGHT_WORD_A.l
postflight_shared_invoke_helper:
                jsr     POSTFLIGHT_HELPER_C.l
