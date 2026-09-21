; Byte-exact tenth record-bank flag gate $C22F7E-$C22F8B.
; The flag-set continuation is outside this slice.

                org     $C22F7E

RECORD_BANK_TEN                 equ $C47584

gate_c22f7e_tenth_record_bank:
                lea     RECORD_BANK_TEN.l,a1
                btst.b  #6,1(a1)
                beq.b   $C22FA8
