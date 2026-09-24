; Byte-exact C346BE-C34709 first filtered fallback byte-pair loop.
                org     $C346BE
SUBMIT_SHARED_RENDERER          equ     $C2F5F4
POSTFLIGHT_FALLBACK_SECOND      equ     $C3470A
POSTFLIGHT_FALLBACK_DONE        equ     $C347EE

submit_postflight_fallback_byte_pairs_first:
                move.b  (a0)+,d0
                move.b  (a0)+,d1
                blt.b   POSTFLIGHT_FALLBACK_SECOND
                neg.b   d1
                ext.w   d0
                ext.w   d1
                add.w   -2(a6),d0
                add.w   -4(a6),d1
                cmpi.w  #14,d0
                ble.b   postflight_fallback_first_next
                cmpi.w  #$132,d0
                bge.b   postflight_fallback_first_next
                cmp.w   -6(a6),d0
                ble.b   postflight_fallback_first_next
                cmp.w   -8(a6),d0
                bge.b   postflight_fallback_first_next
                cmp.w   -10(a6),d1
                ble.b   postflight_fallback_first_next
                cmp.w   -12(a6),d1
                bge.b   postflight_fallback_first_next
                move.l  a0,-(a7)
                jsr     SUBMIT_SHARED_RENDERER.l
                movea.l (a7)+,a0
postflight_fallback_first_next:
                subq.w  #1,-14(a6)
                blt.w   POSTFLIGHT_FALLBACK_DONE
                bra.b   submit_postflight_fallback_byte_pairs_first
