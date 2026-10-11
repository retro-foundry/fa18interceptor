; Byte-exact observed divide-result rounding tail $C259B0-$C259C1.
; The nonnegative fractional path increments the quotient, then both rounding
; paths publish the word result and restore the saved D0-D2 registers.

                org     $C259B0

COORDINATE_QUOTIENT             equ     $C45AD2

finish_divide_result_rounding:
                addq.w  #1,d0
                bra.b   $C259B6
                swap    d0
                move.w  d0,COORDINATE_QUOTIENT
                movem.l (a7)+,d0-d2
                rts
