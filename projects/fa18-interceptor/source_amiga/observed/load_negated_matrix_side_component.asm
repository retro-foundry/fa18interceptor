; Byte-exact observed matrix-side negative-component route $C135B4-$C135D3.
; A signed local index selects a word from the local base table.  The negative
; index route negates that word and publishes it to the shared component slot.

                org     $C135B4

MATRIX_SIDE_SHARED_COMPONENT     equ     $C45B5E

load_negated_matrix_side_component:
                move.w  -$18(a6),d0
                ext.l   d0
                tst.l   d0
                bpl.b   $C135D4
                neg.l   d0
                dc.w    $E380 ; asl.l #1,d0; retain original opcode
                movea.l -$6(a6),a0
                adda.l  d0,a0
                move.w  (a0),d0
                neg.w   d0
                move.w  d0,MATRIX_SIDE_SHARED_COMPONENT.l
                bra.b   $C135E8
