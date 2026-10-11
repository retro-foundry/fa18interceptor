; Byte-exact observed non-$11 postflight setup $C31D8A-$C31DA5.

                org     $C31D8A

POSTFLIGHT_PAIR_SUBMIT          equ     $C2F5C0
POSTFLIGHT_COMMON_SETUP          equ     $C31DE2

prepare_c31d8a_postflight_alternate:
                move.w  #$72,d0
                move.w  #$41,d1
                jsr     POSTFLIGHT_PAIR_SUBMIT.l
                ; LEA $0994.W,A4; observed short-absolute encoding.
                dc.w    $49f8,$0994
                move.w  #$c,d2
                ; LEA -$4A2(PC),A1; observed PC-relative encoding.
                dc.w    $43fa,$fb5e
                bra.b   POSTFLIGHT_COMMON_SETUP
