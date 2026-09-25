; Byte-exact observed basic block $C14960-$C1498F.

                org     $C14960

ACTIVE_RECORD_POINTER           equ     $C18210
RELATIVE_REFERENCE_LONG         equ     $C456FA

prepare_c14960_relative_adjustment:
                move.w  -$6(a6),d0
                ; ASL.W #1,D0; retained as the observed compact encoding.
                dc.w    $e340
                add.w   -$4(a6),d0
                sub.w   -$2(a6),d0
                move.l  RELATIVE_REFERENCE_LONG.l,d1
                movea.l ACTIVE_RECORD_POINTER.l,a0
                sub.l   $18(a0),d1
                move.w  $2(a0),d2
                move.w  d0,-$6(a6)
                move.l  d1,-$a(a6)
                btst    #3,d2
                beq.b   $C1499A
