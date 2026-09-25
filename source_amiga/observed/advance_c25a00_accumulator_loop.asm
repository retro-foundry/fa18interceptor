; Byte-exact observed accumulator-loop tail $C25A00-$C25A07.
; The loop body at $C259FE is outside this entry slice.

                org     $C25A00

ACCUMULATOR_LOOP_BODY          equ     $C259FE

advance_c25a00_accumulator_loop:
                dbf     d6,ACCUMULATOR_LOOP_BODY
                addq.l  #4,a5
                rts
