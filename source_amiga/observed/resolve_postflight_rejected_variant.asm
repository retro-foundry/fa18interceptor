; Byte-exact static-only rejected-variant resolution $C33ED8-$C33F53.
                org     $C33ED8
POSTFLIGHT_STATUS_SOURCE        equ $C45B50
POSTFLIGHT_STATUS_FLAGS         equ $C45B54
POSTFLIGHT_ENABLE_BYTE          equ $C458B4
POSTFLIGHT_MODE_BYTE            equ $C458DB
POSTFLIGHT_SELECTOR             equ $C45954
POSTFLIGHT_STATUS_MASK_GATE     equ $C33DA4
POSTFLIGHT_SETUP_HELPER         equ $C348B2
POSTFLIGHT_VARIANT_HELPER       equ $C31E6C
resolve_postflight_rejected_variant:
                bsr.w   POSTFLIGHT_STATUS_MASK_GATE
                clr.b   POSTFLIGHT_ENABLE_BYTE.l
                moveq   #0,d4
                bra.s   postflight_rejected_finalize
postflight_delta_rejected:
                clr.b   POSTFLIGHT_ENABLE_BYTE.l
                moveq   #0,d4
                move.l  POSTFLIGHT_STATUS_SOURCE.l,d6
                move.l  d6,d5
                andi.l  #$4000,d6
                bne.s   postflight_rejected_update_status
                andi.l  #$200,d5
                beq.s   postflight_rejected_update_status
postflight_rejected_mode_check:
                move.b  POSTFLIGHT_MODE_BYTE.l,d7
                andi.b  #$1F,d7
                cmpi.b  #9,d7
                beq.s   postflight_rejected_update_status
                bra.w   postflight_rejected_finalize
postflight_rejected_update_status:
                andi.l  #$FFFFBFFF,POSTFLIGHT_STATUS_SOURCE.l
                ori.l   #$200,POSTFLIGHT_STATUS_SOURCE.l
                ori.l   #$100,POSTFLIGHT_STATUS_FLAGS.l
postflight_rejected_finalize:
                move.w  #$A,POSTFLIGHT_SELECTOR.l
                bsr.w   POSTFLIGHT_SETUP_HELPER
                tst.b   POSTFLIGHT_ENABLE_BYTE.l
                beq.s   postflight_rejected_done
                jsr     POSTFLIGHT_VARIANT_HELPER.l
postflight_rejected_done:
                rts
