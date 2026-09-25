; Byte-exact output and return block of $C123FA, $C12934-$C1294F.

                org     $C12934

COORDINATE_OUTPUT_X             equ     $C45AC0
COORDINATE_OUTPUT_Z             equ     $C45AC2

publish_c123fa_coordinate_outputs:
                move.l  $8(a6),d0
                move.w  d0,COORDINATE_OUTPUT_X.l
                move.l  $c(a6),d0
                move.w  d0,COORDINATE_OUTPUT_Z.l
                movem.l (sp)+,d2
                unlk    a6
                rts
