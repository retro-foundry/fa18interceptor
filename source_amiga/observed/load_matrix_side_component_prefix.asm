; Byte-exact matrix-side signed-component load $C13574-$C135A1.
; The final BEQ takes the untraced alternate gap and resumes at $C135AC.

                org     $C13574

MATRIX_SIDE_RECORD               equ $C18210

load_matrix_side_component_prefix:
                movea.l MATRIX_SIDE_RECORD.l,a0
                move.b  $28(a0),d0
                ext.w   d0
                move.b  $29(a0),d1
                ext.w   d1
                move.b  $2A(a0),d2
                ext.w   d2
                move.b  $04(a0),d3
                move.w  d0,-$18(a6)
                move.w  d1,-$1A(a6)
                move.w  d2,-$1C(a6)
                btst.l  #3,d3
                dc.w    $670A                   ; beq.b $C135AC
