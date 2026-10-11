; Byte-exact fifth record-bank setup $C22E8A-$C22EBD.
; Downstream update branches remain separately bounded.

                org     $C22E8A

RECORD_INDEX_PRIMARY            equ $C459B4
RECORD_INDEX_SECONDARY          equ $C459B6
RECORD_BANK_FIVE                equ $C46B84
RECORD_BANK_FOUR                equ $C46984
RECORD_UPDATE_STATE_FLAG        equ $C457BB
RECORD_UPDATE_STAGE             equ $C23116

initialize_c22e8a_fifth_record_bank:
                move.w  #5,RECORD_INDEX_PRIMARY.l
                move.w  #$A00,RECORD_INDEX_SECONDARY.l
                lea     RECORD_BANK_FIVE.l,a1
                lea     RECORD_BANK_FOUR.l,a2
                btst.b  #6,1(a1)
                bne.b   $C22EC2
                tst.b   RECORD_UPDATE_STATE_FLAG.l
                bne.b   $C22EBE
                bsr.w   RECORD_UPDATE_STAGE
                bne.b   $C22EC2
                bra.b   $C22ECE
