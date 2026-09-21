; Byte-exact observed setup prefix $C1C63E-$C1C6BB.
; It precedes the complete bounded $C1C63E update-stage packet's C22C80 call.

                org     $C1C63E

STAGE_FLAG_CHANGE               equ $0B
STAGE_INPUT_BYTE                equ $C45854
STAGE_INPUT_BYTE_MIRROR         equ $C45855
STAGE_CHANGE_INHIBIT            equ $C45786
STAGE_SIGNED_LONG               equ $C45A66
STAGE_LONG_MIRROR               equ $C45A6E
STAGE_SCALED_WORD               equ $C45A5E
STAGE_MODE_BYTE                 equ $C458AE
STAGE_LONG_LIMIT                equ $A000
STAGE_SCALE_SHIFT               equ 5
STAGE_MODE_TWO                  equ 2

UPDATE_INDEXED_RECORD_STAGE     equ $C22C80

prepare_c1c63e_update_stage:
                moveq   #0,d5
                move.b  STAGE_INPUT_BYTE.l,d0
                cmp.b   STAGE_INPUT_BYTE_MIRROR.l,d0
                beq.s   .input_synced
                move.b  d0,STAGE_INPUT_BYTE_MIRROR.l
                tst.b   STAGE_CHANGE_INHIBIT.l
                bne.s   .input_synced
                ori.b   #STAGE_FLAG_CHANGE,d5
.input_synced:
                move.l  STAGE_SIGNED_LONG.l,d0
                neg.l   d0
                cmpi.l  #STAGE_LONG_LIMIT,d0
                bge.s   .check_large_long_mirror
                cmpi.l  #STAGE_LONG_LIMIT,STAGE_LONG_MIRROR.l
                bge.s   .mark_long_change
                bra.s   .store_long
.check_large_long_mirror:
                cmpi.l  #STAGE_LONG_LIMIT,STAGE_LONG_MIRROR.l
                bge.s   .store_long
.mark_long_change:
                ori.b   #STAGE_FLAG_CHANGE,d5
.store_long:
                move.l  d0,STAGE_LONG_MIRROR.l
                clr.w   d0
                swap    d0
                asr.l   #STAGE_SCALE_SHIFT,d0
                cmp.w   STAGE_SCALED_WORD.l,d0
                beq.s   .update_record
                cmpi.b  #STAGE_MODE_TWO,STAGE_MODE_BYTE.l
                beq.s   .store_scaled_word
                ori.b   #STAGE_FLAG_CHANGE,d5
.store_scaled_word:
                move.w  d0,STAGE_SCALED_WORD.l
.update_record:
                jsr     UPDATE_INDEXED_RECORD_STAGE.l
