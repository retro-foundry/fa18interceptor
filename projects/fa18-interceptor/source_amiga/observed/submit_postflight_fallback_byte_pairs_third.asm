; Byte-exact C34756-C347A1 third filtered fallback byte-pair loop.
                org     $C34756
SUBMIT_SHARED_RENDERER equ $C2F5F4
FALLBACK_FOURTH equ $C347A2
FALLBACK_DONE equ $C347EE
submit_postflight_fallback_byte_pairs_third:
                addq.w #2,a0
third_body:     move.b (a0)+,d0
                move.b (a0)+,d1
                blt.b FALLBACK_FOURTH
                neg.b d0
                ext.w d0
                ext.w d1
                add.w -2(a6),d0
                add.w -4(a6),d1
                cmpi.w #14,d0
                ble.b third_next
                cmpi.w #$132,d0
                bge.b third_next
                cmp.w -6(a6),d0
                ble.b third_next
                cmp.w -8(a6),d0
                bge.b third_next
                cmp.w -10(a6),d1
                ble.b third_next
                cmp.w -12(a6),d1
                bge.b third_next
                move.l a0,-(a7)
                jsr SUBMIT_SHARED_RENDERER.l
                movea.l (a7)+,a0
third_next:     subq.w #1,-14(a6)
                blt.b FALLBACK_DONE
                bra.b third_body
