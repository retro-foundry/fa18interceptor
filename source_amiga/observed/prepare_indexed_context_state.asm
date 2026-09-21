; Byte-exact indexed context-state preparation $C1BEE8-$C1BF77.

                org     $C1BEE8

REQUEST_LATCH_A                 equ $C45858
REQUEST_LATCH_B                 equ $C458B0
REQUEST_LATCH_A_DISABLED        equ $FF
REQUEST_LATCH_B_VALUE           equ $FE
CONTEXT_WORD_A                  equ $C4593A
CONTEXT_WORD_A_DISABLED         equ $FFFF
CONTEXT_INDEX_WORD              equ $C458DC
CONTEXT_INDEX_OFFSET            equ $C458DE
CONTEXT_STATE_FLAG              equ $C45785
CONTEXT_STATE_COPY_SOURCE       equ $C45833
CONTEXT_MODE                    equ $C45836
CONTEXT_MODE_VALUE              equ 3
CONTEXT_RECORD_BASE             equ $C46184
CONTEXT_RECORD_WORD_OFFSET      equ $68
CONTEXT_RECORD_TYPE_OFFSET      equ $62
CONTEXT_RECORD_TYPE_MASK        equ $F0
CONTEXT_RECORD_TYPE_SPECIAL     equ $30
CONTEXT_OUTPUT_A                equ $C45984
CONTEXT_OUTPUT_B                equ $C45986
CONTEXT_OUTPUT_C                equ $C45988
CONTEXT_OUTPUT_A_MID            equ $A7
CONTEXT_OUTPUT_B_SPECIAL        equ $32
CONTEXT_OUTPUT_C_SPECIAL        equ $320
CONTEXT_INDEX                   equ $C457A7

PREPARE_CONTEXT_OUTPUT          equ $C08324
UPDATE_CONTEXT_OUTPUT           equ $C1BA86
RESET_CONTEXT_COMMAND_INDEX     equ $C1B906
SHARED_COMMAND_QUEUE            equ $C1C23C

prepare_indexed_context_state:
                move.b  #REQUEST_LATCH_A_DISABLED,REQUEST_LATCH_A.l
                move.w  #CONTEXT_WORD_A_DISABLED,CONTEXT_WORD_A.l
                move.b  #REQUEST_LATCH_B_VALUE,REQUEST_LATCH_B.l
                move.w  d1,CONTEXT_INDEX_WORD.l
                asl.w   #8,d1
                add.w   d1,d1
                move.w  d1,CONTEXT_INDEX_OFFSET.l
                jsr     PREPARE_CONTEXT_OUTPUT.l
                tst.b   CONTEXT_STATE_FLAG.l
                beq.b   .load_context_record
                move.b  CONTEXT_STATE_COPY_SOURCE.l,CONTEXT_STATE_FLAG.l
                move.b  #CONTEXT_MODE_VALUE,CONTEXT_MODE.l
                bra.w   SHARED_COMMAND_QUEUE
.load_context_record:
                lea.l   CONTEXT_RECORD_BASE.l,a0
                move.w  CONTEXT_RECORD_WORD_OFFSET(a0,d1.w),$C4592A.l
                move.b  CONTEXT_RECORD_TYPE_OFFSET(a0,d1.w),d4
                andi.b  #CONTEXT_RECORD_TYPE_MASK,d4
                cmpi.b  #CONTEXT_RECORD_TYPE_SPECIAL,d4
                bne.w   RESET_CONTEXT_COMMAND_INDEX
                move.w  #CONTEXT_OUTPUT_B_SPECIAL,CONTEXT_OUTPUT_B.l
                move.w  #CONTEXT_OUTPUT_C_SPECIAL,CONTEXT_OUTPUT_C.l
                clr.b   CONTEXT_INDEX.l
                bsr.w   UPDATE_CONTEXT_OUTPUT
                move.w  #CONTEXT_OUTPUT_A_MID,CONTEXT_OUTPUT_A.l
                bra.w   SHARED_COMMAND_QUEUE
