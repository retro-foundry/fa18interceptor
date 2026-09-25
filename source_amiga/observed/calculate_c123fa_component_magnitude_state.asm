; Byte-exact observed continuation in $C123FA, $C125FC-$C12685.
; The surrounding coordinate-update frame supplies the shifted components.

                org     $C125FC

COMPONENT_SQUARE_SUM            equ     $C45B64
COMPONENT_THRESHOLD              equ     $C45B68
COORDINATE_DIVIDEND              equ     $C45ACC
COORDINATE_DIVISOR               equ     $C45AD0
COORDINATE_QUOTIENT              equ     $C45AD2
COORDINATE_TABLE                 equ     $C3DB00
COORDINATE_DIVIDE_HELPER         equ     $C25980
MAGNITUDE_UPDATE_HELPER          equ     $C2564E

calculate_c123fa_component_magnitude_state:
                move.w  -$18(a6),d0
                ext.l   d0
                move.l  $18(a6),d1
                asr.l   d0,d1
                move.w  d1,-$8(a6)
                muls.w  d1,d1
                move.l  $10(a6),d2
                asr.l   d0,d2
                move.w  d2,-$8(a6)
                muls.w  d2,d2
                move.l  d1,-$16(a6)
                add.l   d2,d1
                move.l  d1,COMPONENT_SQUARE_SUM.l
                jsr     MAGNITUDE_UPDATE_HELPER.l
                move.w  -$18(a6),d0
                ext.l   d0
                move.l  $14(a6),d1
                asr.l   d0,d1
                move.w  d1,-$2(a6)
                ext.l   d1
                move.w  COMPONENT_THRESHOLD.l,d0
                ext.l   d0
                cmp.l   d0,d1
                bgt.b   $C12690
                asl.l   #8,d1
                move.l  d1,COORDINATE_DIVIDEND.l
                move.w  COMPONENT_THRESHOLD.l,d0
                move.w  d0,COORDINATE_DIVISOR.l
                tst.w   d0
                beq.b   $C12686
                jsr     COORDINATE_DIVIDE_HELPER.l
                move.w  COORDINATE_QUOTIENT.l,d0
                ext.l   d0
                ; ASL.L #1,D0; retain the original immediate encoding.
                dc.w    $E380
                movea.l d0,a0
                adda.l  #COORDINATE_TABLE,a0
                move.w  (a0),d0
                ext.l   d0
                asl.l   #3,d0
                move.l  d0,-$e(a6)
                bra.b   $C126D2
