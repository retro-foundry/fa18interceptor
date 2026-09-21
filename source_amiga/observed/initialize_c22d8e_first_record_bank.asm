; Byte-exact first record-bank setup $C22D8E-$C22DB3.
; Downstream update branches remain separately bounded.

                org     $C22D8E

RECORD_INDEX_PRIMARY            equ $C459B4
RECORD_INDEX_SECONDARY          equ $C459B6
RECORD_BANK_ONE                 equ $C46384
RECORD_BANK_BASE                equ $C46184
RECORD_UPDATE_STATE_FLAG        equ $C457BA

initialize_c22d8e_first_record_bank:
                move.w  #1,RECORD_INDEX_PRIMARY.l
                move.w  #$200,RECORD_INDEX_SECONDARY.l
                lea     RECORD_BANK_ONE.l,a1
                lea     RECORD_BANK_BASE.l,a2
                btst.b  #6,1(a1)
                bne.w   $C22DC8
                tst.b   RECORD_UPDATE_STATE_FLAG.l
                bne.b   $C22DC4
                bsr.w   $C230E8
                bne.b   $C22DC8
                bra.b   $C22DD4
