; Byte-exact structural guard at $C24FE8, observed during run024's crash result.
; The meaning and owner of $C457D7 remain unassigned.
                org     $C24FE6

return_if_c457d7_zero:
                rts

guard_nonzero_c457d7:
                tst.b   $C457D7
                beq.s   return_if_c457d7_zero
