; Byte-exact C3466A-C34695 fourth byte-pair loop and linked-frame epilogue.
                org     $C3466A
SUBMIT_SHARED_RENDERER          equ     $C2F5F4

submit_postflight_byte_pairs_fourth:
                subq.w  #2,a0
postflight_byte_pair_fourth_body:
                move.b  -(a0),d1
                move.b  -(a0),d0
                blt.b   postflight_byte_pair_fourth_done
                neg.b   d0
                neg.b   d1
                ext.w   d0
                ext.w   d1
                add.w   -2(a6),d0
                add.w   -4(a6),d1
                move.l  a0,-(a7)
                jsr     SUBMIT_SHARED_RENDERER.l
                movea.l (a7)+,a0
                subq.w  #1,-14(a6)
                bge.b   postflight_byte_pair_fourth_body
postflight_byte_pair_fourth_done:
                unlk    a6
                rts
