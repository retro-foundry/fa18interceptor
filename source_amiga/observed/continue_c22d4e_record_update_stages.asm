; Byte-exact record-update stage continuation $C22D4E-$C22D8D.
; Callee semantics remain separately bounded.

                org     $C22D4E

RECORD_SETUP_HELPER             equ $C230B0
RECORD_INDEX_PRIMARY            equ $C459B4
RECORD_INDEX_SECONDARY          equ $C459B6
RECORD_TABLE_PRIMARY            equ $C46184
RECORD_TABLE_SECONDARY          equ $C46984
RECORD_STRIDE_STATE_FLAG        equ $C457AE
RECORD_UPDATE_STAGE_A           equ $C23228
RECORD_UPDATE_STAGE_B           equ $C23CA6
RECORD_UPDATE_STAGE_C           equ $C244E2
RECORD_UPDATE_STAGE_D           equ $C25B66

continue_c22d4e_record_update_stages:
                bsr.w   RECORD_SETUP_HELPER
                clr.w   RECORD_INDEX_PRIMARY.l
                clr.w   RECORD_INDEX_SECONDARY.l
                lea     RECORD_TABLE_PRIMARY.l,a1
                lea     RECORD_TABLE_SECONDARY.l,a2
                tst.b   RECORD_STRIDE_STATE_FLAG.l
                bne.b   $C22D76
                subq.w  #1,$4C(a1)
                dc.w    $0269,$FFFD,0           ; andi.w  #-$3,0(a1)
                bsr.w   RECORD_UPDATE_STAGE_A
                bsr.w   RECORD_UPDATE_STAGE_B
                bsr.w   RECORD_UPDATE_STAGE_C
                jsr     RECORD_UPDATE_STAGE_D.l
