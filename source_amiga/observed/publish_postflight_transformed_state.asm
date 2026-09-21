; Byte-exact static-only transformed-state publication $C33D3A-$C33DA3.
                org     $C33D3A
POSTFLIGHT_VARIANT_PAIR         equ $C4593E
POSTFLIGHT_CURRENT_VECTOR       equ $C45716
POSTFLIGHT_REFERENCE_VECTOR     equ $C45722
publish_postflight_transformed_state:
                movem.w d0-d1,POSTFLIGHT_VARIANT_PAIR.l
                move.w  #$600,d4
                move.w  #$4000,d5
                lea.l   (a1),a2
                dc.w    $D4FC,$0092 ; adda.w #$92,a2
                move.w  d4,d0
                move.w  d5,d7
                muls.w  $2(a2),d0
                muls.w  $4(a2),d7
                add.l   d7,d0
                move.w  d4,d1
                move.w  d5,d7
                muls.w  $8(a2),d1
                muls.w  $A(a2),d7
                add.l   d7,d1
                move.w  d4,d2
                muls.w  $E(a2),d2
                muls.w  $10(a2),d5
                add.l   d5,d2
                asr.l   #6,d0
                asr.l   #6,d1
                asr.l   #6,d2
                add.l   $14(a1),d0
                add.l   $18(a1),d1
                add.l   $1C(a1),d2
                movem.l POSTFLIGHT_CURRENT_VECTOR.l,d3-d5
                movem.l d3-d5,POSTFLIGHT_REFERENCE_VECTOR.l
                movem.l d0-d2,POSTFLIGHT_CURRENT_VECTOR.l
                rts
