; Byte-exact observed setup block in $C279D0, $C27A36-$C27A6D.
; The BLT target at $C27A6E remains outside this observed block.

                org     $C27A36

PROJECTION_COMPONENT_X          equ     $C45A72
PROJECTION_COMPONENT_Y          equ     $C45A76
PROJECTION_GRID_TABLE            equ     $C28124
PROJECTION_GRID_CONTINUE         equ     $C27A80
PROJECTION_GRID_ALTERNATE        equ     $C27A6E

prepare_c279d0_projection_grid:
                move.w  PROJECTION_COMPONENT_X.l,d0
                move.w  PROJECTION_COMPONENT_Y.l,d2
                movem.w d0-d2,-$6(a6)
                move.w  #-$200,d1
                move.w  #-$80,d3
                move.w  #-$e0,d4
                move.w  -$4(a6),d0
                clr.w   -$16(a6)
                cmp.w   d3,d0
                blt.b   PROJECTION_GRID_ALTERNATE
                lea     PROJECTION_GRID_TABLE.l,a3
                move.w  #3,-$16(a6)
                bra.b   PROJECTION_GRID_CONTINUE
