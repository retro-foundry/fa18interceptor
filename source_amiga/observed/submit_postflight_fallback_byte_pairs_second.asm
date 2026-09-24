; Byte-exact C3470A-C34755 second filtered fallback byte-pair loop.
                org     $C3470A
SUBMIT_SHARED_RENDERER          equ     $C2F5F4
POSTFLIGHT_FALLBACK_THIRD       equ     $C34756
POSTFLIGHT_FALLBACK_DONE        equ     $C347EE

submit_postflight_fallback_byte_pairs_second:
                subq.w  #2,a0
postflight_fallback_second_body:
                move.b  -(a0),d1
                move.b  -(a0),d0
                blt.b   POSTFLIGHT_FALLBACK_THIRD
                ext.w   d0
                ext.w   d1
                add.w   -2(a6),d0
                add.w   -4(a6),d1
                cmpi.w  #14,d0
                ble.b   postflight_fallback_second_next
                cmpi.w  #$132,d0
                bge.b   postflight_fallback_second_next
                cmp.w   -6(a6),d0
                ble.b   postflight_fallback_second_next
                cmp.w   -8(a6),d0
                bge.b   postflight_fallback_second_next
                cmp.w   -10(a6),d1
                ble.b   postflight_fallback_second_next
                cmp.w   -12(a6),d1
                bge.b   postflight_fallback_second_next
                move.l  a0,-(a7)
                jsr     SUBMIT_SHARED_RENDERER.l
                movea.l (a7)+,a0
postflight_fallback_second_next:
                subq.w  #1,-14(a6)
                blt.w   POSTFLIGHT_FALLBACK_DONE
                bra.b   postflight_fallback_second_body
