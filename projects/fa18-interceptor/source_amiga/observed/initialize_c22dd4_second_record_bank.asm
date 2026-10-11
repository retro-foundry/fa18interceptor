; Byte-exact second record-bank setup $C22DD4-$C22E17.
; Downstream update branches remain separately bounded.

                org     $C22DD4

RECORD_INDEX_PRIMARY            equ $C459B4
RECORD_INDEX_SECONDARY          equ $C459B6
RECORD_BANK_TWO                 equ $C46584
RECORD_BANK_BASE                equ $C46184
RECORD_UPDATE_STATE_FLAG        equ $C457BA

initialize_c22dd4_second_record_bank:
                move.w  #2,RECORD_INDEX_PRIMARY.l
                move.w  #$400,RECORD_INDEX_SECONDARY.l
                lea     RECORD_BANK_TWO.l,a1
                lea     RECORD_BANK_BASE.l,a2
                btst.b  #6,1(a1)
                bne.b   $C22E0C
                tst.b   RECORD_UPDATE_STATE_FLAG.l
                bne.b   $C22E08
                bsr.w   $C230E8
                bne.b   $C22E0C
                bra.b   $C22E18
