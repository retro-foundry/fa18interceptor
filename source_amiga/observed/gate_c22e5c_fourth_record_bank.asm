; Byte-exact fourth record-bank flag gate $C22E5C-$C22E69.
; The flag-set continuation is outside this slice.

                org     $C22E5C

RECORD_BANK_FOUR                equ $C46984

gate_c22e5c_fourth_record_bank:
                lea     RECORD_BANK_FOUR.l,a1
                btst.b  #6,1(a1)
                beq.b   $C22E8A
