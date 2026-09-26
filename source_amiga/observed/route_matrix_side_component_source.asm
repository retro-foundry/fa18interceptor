; Byte-exact observed matrix-side component-source route $C135E8-$C13617.
; A bit in the local source record selects either a signed shared component
; passed through the C13BA0 helper, or a bit-0 gate on the active record +$20.

                org     $C135E8

MATRIX_SIDE_SHARED_COMPONENT     equ     $C45B5E
CURRENT_CONTROL_RECORD           equ     $C18210
MATRIX_SIDE_COMPONENT_HELPER     equ     $C13BA0

route_matrix_side_component_source:
                movea.l -$A(a6),a0
                move.w  (a0),d0
                btst    #7,d0
                bne.b   $C13608
                move.w  MATRIX_SIDE_SHARED_COMPONENT.l,d0
                ext.l   d0
                move.l  d0,-(a7)
                bsr.w   MATRIX_SIDE_COMPONENT_HELPER
                addq.l  #4,a7
                bra.w   $C1371E
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.b  $20(a0),d0
                btst    #0,d0
                beq.b   $C13638
