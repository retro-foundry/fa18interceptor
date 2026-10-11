; Byte-exact final record-bank flag gate $C23076-$C23083.
; The flag-set continuation is outside this slice.

                org     $C23076

RECORD_BANK_FINAL               equ $C47F84

gate_c23076_final_record_bank:
                lea     RECORD_BANK_FINAL.l,a1
                btst.b  #6,1(a1)
                beq.b   $C230A6
