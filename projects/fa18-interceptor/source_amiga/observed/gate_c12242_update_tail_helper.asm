; Byte-exact observed flag gate at $C12242-$C12249.
; The nonzero continuation is not covered by this bounded slice.

                org     $C12242

UPDATE_TAIL_GATE_WORD           equ     $C458DC
UPDATE_TAIL_GATE_RETURN         equ     $C122A0

gate_c12242_update_tail_helper:
                tst.w   UPDATE_TAIL_GATE_WORD.l
                beq.b   UPDATE_TAIL_GATE_RETURN
