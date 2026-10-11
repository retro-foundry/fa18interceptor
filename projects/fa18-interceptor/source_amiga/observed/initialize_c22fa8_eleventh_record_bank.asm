; Byte-exact eleventh record-bank setup $C22FA8-$C22FD1.
; Downstream update branches remain separately bounded.

                org     $C22FA8

RECORD_INDEX_PRIMARY            equ $C459B4
RECORD_INDEX_SECONDARY          equ $C459B6
RECORD_BANK_ELEVEN              equ $C47784
RECORD_BANK_TEN                 equ $C47584
RECORD_UPDATE_STAGE             equ $C231A2

initialize_c22fa8_eleventh_record_bank:
                move.w  #$B,RECORD_INDEX_PRIMARY.l
                move.w  #$1600,RECORD_INDEX_SECONDARY.l
                lea     RECORD_BANK_ELEVEN.l,a1
                lea     RECORD_BANK_TEN.l,a2
                btst.b  #6,1(a1)
                bne.b   $C22FD6
                bsr.w   RECORD_UPDATE_STAGE
                beq.b   $C22FE2
