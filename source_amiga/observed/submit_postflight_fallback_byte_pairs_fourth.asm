; Byte-exact C347A2-C347F1 fourth filtered fallback loop and epilogue.
                org     $C347A2
SUBMIT_SHARED_RENDERER equ $C2F5F4
submit_postflight_fallback_byte_pairs_fourth:
                subq.w #2,a0
fourth_body:     move.b -(a0),d1
                move.b -(a0),d0
                blt.b fourth_done
                neg.b d0
                neg.b d1
                ext.w d0
                ext.w d1
                add.w -2(a6),d0
                add.w -4(a6),d1
                cmpi.w #14,d0
                ble.b fourth_next
                cmpi.w #$132,d0
                bge.b fourth_next
                cmp.w -6(a6),d0
                ble.b fourth_next
                cmp.w -8(a6),d0
                bge.b fourth_next
                cmp.w -10(a6),d1
                ble.b fourth_next
                cmp.w -12(a6),d1
                bge.b fourth_next
                move.l a0,-(a7)
                jsr SUBMIT_SHARED_RENDERER.l
                movea.l (a7)+,a0
fourth_next:     subq.w #1,-14(a6)
                bge.b fourth_body
fourth_done:     unlk a6
                rts
