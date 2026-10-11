; Byte-exact fourteenth record-bank update $C23046-$C23075.
; Callee semantics remain separately bounded.

                org     $C23046

RECORD_BANK_FOURTEEN            equ $C47D84
RECORD_INDEX_PRIMARY            equ $C459B4
RECORD_INDEX_SECONDARY          equ $C459B6
RECORD_UPDATE_STAGE             equ $C23A7E
INDEXED_UPDATE_STAGE            equ $C25B66

update_c23046_fourteenth_record_bank:
                lea     RECORD_BANK_FOURTEEN.l,a1
                btst.b  #6,1(a1)
                beq.b   $C23076
                bset.b  #2,1(a1)
                move.w  #$E,RECORD_INDEX_PRIMARY.l
                move.w  #$1C00,RECORD_INDEX_SECONDARY.l
                bsr.w   RECORD_UPDATE_STAGE
                beq.b   $C23076
                jsr     INDEXED_UPDATE_STAGE.l
