; Byte-exact observed C13D84 continuation $C14BF2-$C14C01.
; A zero local -$24 permits the shared byte mode to select the following
; signed-local adjustment route; nonzero skips directly to its join.

                org     $C14BF2

CONTROL_ADJUSTMENT_MODE        equ     $C4579F

gate_c13d84_local_mode_adjustment:
                tst.w   -$24(a6)
                bne.b   $C14C1E
                move.b  CONTROL_ADJUSTMENT_MODE.l,d0
                subq.b  #3,d0
                bne.b   $C14C0C
