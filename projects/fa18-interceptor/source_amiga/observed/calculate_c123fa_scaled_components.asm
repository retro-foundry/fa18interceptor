; Byte-exact observed continuation in $C123FA, $C1256A-$C125C3.

                org     $C1256A

COORDINATE_DIVIDEND             equ     $C45ACC
COORDINATE_DIVISOR              equ     $C45AD0
COORDINATE_QUOTIENT             equ     $C45AD2
COORDINATE_TABLE                equ     $C3DB00
COORDINATE_DIVIDE_HELPER        equ     $C25980

calculate_c123fa_scaled_components:
                move.w  -$1a(a6),d0
                ext.l   d0
                move.l  $18(a6),d1
                asl.l   d0,d1
                move.l  d1,COORDINATE_DIVIDEND.l
                move.w  -$4(a6),COORDINATE_DIVISOR.l
                jsr     COORDINATE_DIVIDE_HELPER.l
                move.w  COORDINATE_QUOTIENT.l,d0
                asr.w   #6,d0
                move.w  d0,COORDINATE_QUOTIENT.l
                ext.l   d0
                ; ASL.L #1,D0; retain the original immediate encoding.
                dc.w    $E380
                movea.l d0,a0
                adda.l  #COORDINATE_TABLE,a0
                move.w  (a0),d0
                ext.l   d0
                move.l  #$384,d1
                sub.l   d0,d1
                asl.l   #3,d1
                move.l  d1,-$12(a6)
                move.b  -$9(a6),d0
                andi.b  #5,d0
                move.b  d0,-$a(a6)
                subq.b  #5,d0
                bne.b   $C125D6
