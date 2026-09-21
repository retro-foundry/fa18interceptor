; Byte-exact sixth record-bank flag gate $C22ECE-$C22EDB.
; The flag-set continuation is outside this slice.

                org     $C22ECE

RECORD_BANK_SIX                 equ $C46D84

gate_c22ece_sixth_record_bank:
                lea     RECORD_BANK_SIX.l,a1
                btst.b  #6,1(a1)
                beq.b   $C22EF8
