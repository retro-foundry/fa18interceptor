; Byte-exact matrix-side record bit-8 table selector $C13488-$C1348F.
; A clear bit selects the alternate table pointer at $C1349A.

                org     $C13488

select_matrix_side_record_bit8_table:
                move.w  (a0),d0
                dc.w    $0800,$0008             ; btst.b #8,d0
                beq.b   $C1349A
