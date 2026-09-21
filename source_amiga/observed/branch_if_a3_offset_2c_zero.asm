; Byte-exact observed entry prefix at $C50212.
; A3 record ownership and the meaning of its +$2C longword are unassigned.
                org     $C50212

ZERO_A3_OFFSET_2C_RETURN        equ $C5027A

branch_if_a3_offset_2c_zero:
                ; Preserve CMPi.L #0,(d16,A3); vasm otherwise emits TST.L.
                dc.w    $0CAB,0,0,$2C
                beq.s   ZERO_A3_OFFSET_2C_RETURN
