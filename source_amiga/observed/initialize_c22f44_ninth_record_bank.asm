; Byte-exact ninth record-bank setup $C22F44-$C22F6D.
; The helper-success continuation starts outside this slice at $C22F6E.

                org     $C22F44

RECORD_INDEX_PRIMARY            equ $C459B4
RECORD_INDEX_SECONDARY          equ $C459B6
RECORD_BANK_NINE                equ $C47384
RECORD_BANK_EIGHT               equ $C47184
RECORD_UPDATE_STAGE             equ $C231A2

initialize_c22f44_ninth_record_bank:
                move.w  #9,RECORD_INDEX_PRIMARY.l
                move.w  #$1200,RECORD_INDEX_SECONDARY.l
                lea     RECORD_BANK_NINE.l,a1
                lea     RECORD_BANK_EIGHT.l,a2
                btst.b  #6,1(a1)
                bne.b   $C22F72
                bsr.w   RECORD_UPDATE_STAGE
                beq.b   $C22F7E
