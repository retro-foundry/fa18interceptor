; Byte-exact thirteenth record-bank setup $C2300C-$C23045.
; Downstream update branches remain separately bounded.

                org     $C2300C

RECORD_INDEX_PRIMARY            equ $C459B4
RECORD_INDEX_SECONDARY          equ $C459B6
RECORD_BANK_THIRTEEN            equ $C47B84
RECORD_BANK_TWELVE              equ $C47984
RECORD_UPDATE_STAGE             equ $C231A2

initialize_c2300c_thirteenth_record_bank:
                move.w  #$D,RECORD_INDEX_PRIMARY.l
                move.w  #$1A00,RECORD_INDEX_SECONDARY.l
                lea     RECORD_BANK_THIRTEEN.l,a1
                lea     RECORD_BANK_TWELVE.l,a2
                btst.b  #6,1(a1)
                bne.b   $C2303A
                bsr.w   RECORD_UPDATE_STAGE
                beq.b   $C23046
