; Byte-exact static-only record transform $C33CD2-$C33D39.
                org     $C33CD2
POSTFLIGHT_RECORD_BASE          equ $C46184
POSTFLIGHT_RECORD_OFFSET        equ $C458DE
POSTFLIGHT_REFERENCE_VECTOR     equ $C45722
POSTFLIGHT_COEFFICIENTS         equ $C45BD8
POSTFLIGHT_TRANSFORM_HELPER     equ $C2EC90
compute_postflight_record_transform:
                lea.l   POSTFLIGHT_RECORD_BASE.l,a1
                adda.w  POSTFLIGHT_RECORD_OFFSET.l,a1
                movem.l POSTFLIGHT_REFERENCE_VECTOR.l,d2-d4
                sub.l   $14(a1),d2
                sub.l   $18(a1),d3
                sub.l   $1C(a1),d4
                asr.l   #8,d2
                asr.l   #8,d3
                asr.l   #8,d4
                lea.l   POSTFLIGHT_COEFFICIENTS.l,a2
                move.w  d2,d5
                move.w  d3,d6
                move.w  d4,d7
                muls.w  (a2)+,d5
                muls.w  (a2)+,d6
                muls.w  (a2)+,d7
                add.l   d6,d7
                add.l   d5,d7
                asr.l   #8,d7
                move.w  d7,d0
                move.w  d2,d5
                move.w  d3,d6
                move.w  d4,d7
                muls.w  (a2)+,d5
                muls.w  (a2)+,d6
                muls.w  (a2)+,d7
                add.l   d6,d7
                add.l   d5,d7
                asr.l   #8,d7
                muls.w  (a2)+,d2
                muls.w  (a2)+,d3
                muls.w  (a2)+,d4
                add.l   d3,d4
                add.l   d2,d4
                asr.l   #8,d4
                move.w  d7,d1
                move.w  d4,d2
                jsr     POSTFLIGHT_TRANSFORM_HELPER.l
