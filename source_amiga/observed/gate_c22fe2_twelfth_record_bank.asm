; Byte-exact twelfth record-bank flag gate $C22FE2-$C22FEF.
; The flag-set continuation is outside this slice.

                org     $C22FE2

RECORD_BANK_TWELVE              equ $C47984

gate_c22fe2_twelfth_record_bank:
                lea     RECORD_BANK_TWELVE.l,a1
                btst.b  #6,1(a1)
                beq.b   $C2300C
