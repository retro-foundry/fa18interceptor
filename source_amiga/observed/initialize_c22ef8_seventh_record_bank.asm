; Byte-exact seventh record-bank setup $C22EF8-$C22F25.
; Downstream record-bank handling remains separately bounded.

                org     $C22EF8

RECORD_INDEX_PRIMARY            equ $C459B4
RECORD_INDEX_SECONDARY          equ $C459B6
RECORD_BANK_SEVEN               equ $C46F84
RECORD_BANK_SIX                 equ $C46D84
RECORD_BANK_EIGHT               equ $C47184

initialize_c22ef8_seventh_record_bank:
                move.w  #7,RECORD_INDEX_PRIMARY.l
                move.w  #$E00,RECORD_INDEX_SECONDARY.l
                lea     RECORD_BANK_SEVEN.l,a1
                lea     RECORD_BANK_SIX.l,a2
                btst.b  #6,1(a1)
                lea     RECORD_BANK_EIGHT.l,a1
                btst.b  #6,1(a1)
                beq.b   $C22F44
