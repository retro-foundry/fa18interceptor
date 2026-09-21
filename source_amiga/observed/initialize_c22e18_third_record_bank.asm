; Byte-exact third record-bank setup $C22E18-$C22E4F.
; Downstream update branches remain separately bounded.

                org     $C22E18

RECORD_INDEX_PRIMARY            equ $C459B4
RECORD_INDEX_SECONDARY          equ $C459B6
RECORD_BANK_THREE               equ $C46784
RECORD_BANK_BASE                equ $C46184
RECORD_UPDATE_STATE_FLAG        equ $C457BA

initialize_c22e18_third_record_bank:
                move.w  #3,RECORD_INDEX_PRIMARY.l
                move.w  #$600,RECORD_INDEX_SECONDARY.l
                lea     RECORD_BANK_THREE.l,a1
                lea     RECORD_BANK_BASE.l,a2
                btst.b  #6,1(a1)
                bne.b   $C22E50
                tst.b   RECORD_UPDATE_STATE_FLAG.l
                bne.b   $C22E4C
                bsr.w   $C230E8
                bne.b   $C22E50
