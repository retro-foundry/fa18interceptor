; Byte-exact C345F0-C34617 forward byte-pair submission loop.
                org     $C345F0
SUBMIT_SHARED_RENDERER          equ     $C2F5F4
POSTFLIGHT_BYTE_PAIR_SECOND     equ     $C34618
POSTFLIGHT_BYTE_PAIR_DONE       equ     $C34692

submit_postflight_byte_pairs_forward:
                move.b  (a0)+,d0
                move.b  (a0)+,d1
                blt.b   POSTFLIGHT_BYTE_PAIR_SECOND
                neg.b   d1
                ext.w   d0
                ext.w   d1
                add.w   -2(a6),d0
                add.w   -4(a6),d1
                move.l  a0,-(a7)
                jsr     SUBMIT_SHARED_RENDERER.l
                movea.l (a7)+,a0
                subq.w  #1,-14(a6)
                blt.w   POSTFLIGHT_BYTE_PAIR_DONE
                bra.b   submit_postflight_byte_pairs_forward
