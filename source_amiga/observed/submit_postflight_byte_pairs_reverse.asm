; Byte-exact C34618-C3463F reverse byte-pair submission loop.
                org     $C34618
SUBMIT_SHARED_RENDERER          equ     $C2F5F4
POSTFLIGHT_BYTE_PAIR_THIRD      equ     $C34640
POSTFLIGHT_BYTE_PAIR_DONE       equ     $C34692

submit_postflight_byte_pairs_reverse:
                subq.w  #2,a0
postflight_byte_pair_reverse_body:
                move.b  -(a0),d1
                move.b  -(a0),d0
                blt.b   POSTFLIGHT_BYTE_PAIR_THIRD
                subq.b  #1,d1
                ext.w   d0
                ext.w   d1
                add.w   -2(a6),d0
                add.w   -4(a6),d1
                move.l  a0,-(a7)
                jsr     SUBMIT_SHARED_RENDERER.l
                movea.l (a7)+,a0
                subq.w  #1,-14(a6)
                blt.b   POSTFLIGHT_BYTE_PAIR_DONE
                bra.b   postflight_byte_pair_reverse_body
