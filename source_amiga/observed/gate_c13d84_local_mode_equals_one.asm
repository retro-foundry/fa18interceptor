; Byte-exact observed C13D84 continuation $C14C0C-$C14C15.
; It tests whether the shared local-adjustment mode equals one, otherwise
; joining the common $C14C1E continuation.

                org     $C14C0C

CONTROL_ADJUSTMENT_MODE        equ     $C4579F

gate_c13d84_local_mode_equals_one:
                move.b  CONTROL_ADJUSTMENT_MODE.l,d0
                subq.b  #1,d0
                bne.b   $C14C1E
