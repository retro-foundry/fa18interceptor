; Byte-exact observed $11-type postflight setup $C31DA6-$C31DE1.

                org     $C31DA6

POSTFLIGHT_FORMATTER            equ     $C32AB4
POSTFLIGHT_PAIR_SUBMIT          equ     $C2F5C0

prepare_c31da6_postflight_type11:
                ; LEA $C31E5E.L,A2; preserve the original absolute-long form.
                dc.w    $45f9,$00c3,$1e5e
                lea     $1(a2),a0
                ; LEA -$49A(PC),A1; observed PC-relative encoding.
                dc.w    $43fa,$fb66
                ; LEA $12F2.W,A4; observed short-absolute encoding.
                dc.w    $49f8,$12f2
                move.w  #$a,d0
                swap    d0
                move.w  #0,d0
                jsr     POSTFLIGHT_FORMATTER.l
                move.w  #$64,d0
                move.w  #$7d,d1
                jsr     POSTFLIGHT_PAIR_SUBMIT.l
                ; LEA $12F2.W,A4; observed short-absolute encoding.
                dc.w    $49f8,$12f2
                move.w  #$a,d2
                ; LEA -$4D4(PC),A1; observed PC-relative encoding.
                dc.w    $43fa,$fb2c
