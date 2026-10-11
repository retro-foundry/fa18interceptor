; Byte-exact tenth record-bank update $C22F8C-$C22FA7.
; Callee semantics remain separately bounded.

                org     $C22F8C

RECORD_INDEX_PRIMARY            equ $C459B4
RECORD_INDEX_SECONDARY          equ $C459B6
RECORD_UPDATE_STAGE             equ $C23A7E
INDEXED_UPDATE_STAGE            equ $C25B66

update_c22f8c_tenth_record_bank:
                move.w  #$A,RECORD_INDEX_PRIMARY.l
                move.w  #$1400,RECORD_INDEX_SECONDARY.l
                bsr.w   RECORD_UPDATE_STAGE
                beq.b   $C22FA8
                jsr     INDEXED_UPDATE_STAGE.l
