; Byte-exact context command selection publisher $C1B9CC-$C1BAD3.

                org     $C1B9CC

CONTEXT_INDEX                   equ $C457A7
CONTEXT_INDEX_CLEAR             equ $C457A9
CONTEXT_INDEX_CLEAR_VALUE       equ 0
CONTEXT_MODE                    equ $C45836
CONTEXT_MODE_VALUE              equ 3
CONTEXT_RECORD_BASE             equ $C46184
CONTEXT_RECORD_INDEX            equ $C458DE
CONTEXT_RECORD_TYPE_OFFSET      equ $62
CONTEXT_RECORD_TYPE_MASK        equ $F0
CONTEXT_RECORD_TYPE_SPECIAL     equ $30
CONTEXT_WORD_A                  equ $C45936
CONTEXT_WORD_A_DISABLED         equ $FFFF
CONTEXT_LATCH_A                 equ $C45858
CONTEXT_LATCH_B                 equ $C45891
CONTEXT_LATCH_DISABLED          equ $FF
CONTEXT_OUTPUT_A                equ $C45984
CONTEXT_OUTPUT_B                equ $C45986
CONTEXT_OUTPUT_C                equ $C45988
CONTEXT_OUTPUT_A_LOW            equ $90
CONTEXT_OUTPUT_A_MID            equ $A7
CONTEXT_OUTPUT_A_HIGH           equ $B3
CONTEXT_OUTPUT_B_SPECIAL        equ $32
CONTEXT_OUTPUT_C_SPECIAL        equ $320
CONTEXT_INDEX_THREE             equ 3
CONTEXT_INDEX_FOUR              equ 4
CONTEXT_INDEX_FIVE              equ 5
CONTEXT_INDEX_SEVEN             equ 7
CONTEXT_INDEX_EIGHT             equ 8
CONTEXT_INDEX_NINE              equ 9
CONTEXT_INDEX_TWELVE            equ $0C

CONTEXT_OUTPUT_LOOKUP_TABLE     equ $C1BAD4
PREPARE_CONTEXT_OUTPUT          equ $C08324
UPDATE_CONTEXT_OUTPUT           equ $C082B8
SHARED_COMMAND_QUEUE            equ $C1C23C

publish_context_command_selection:
                move.b  d7,CONTEXT_INDEX.l
                move.b  #CONTEXT_MODE_VALUE,CONTEXT_MODE.l
continue_context_command_selection:
                jsr     PREPARE_CONTEXT_OUTPUT.l
                move.w  #CONTEXT_WORD_A_DISABLED,CONTEXT_WORD_A.l
                move.b  #CONTEXT_LATCH_DISABLED,CONTEXT_LATCH_A.l
                move.b  #CONTEXT_INDEX_CLEAR_VALUE,CONTEXT_INDEX_CLEAR.l
                move.b  #CONTEXT_LATCH_DISABLED,CONTEXT_LATCH_B.l
                lea.l   CONTEXT_RECORD_BASE.l,a0
                adda.w  CONTEXT_RECORD_INDEX.l,a0
                move.b  CONTEXT_RECORD_TYPE_OFFSET(a0),d4
                andi.b  #CONTEXT_RECORD_TYPE_MASK,d4
                cmpi.b  #CONTEXT_RECORD_TYPE_SPECIAL,d4
                beq.w   SHARED_COMMAND_QUEUE
                move.b  CONTEXT_INDEX.l,d4
                cmpi.b  #CONTEXT_INDEX_THREE,d4
                blt.b   .lookup_output
                cmpi.b  #CONTEXT_INDEX_NINE,d4
                ble.b   .select_high_output
                cmpi.b  #CONTEXT_INDEX_TWELVE,d4
                blt.b   .lookup_output
.select_high_output:
                move.w  #CONTEXT_OUTPUT_A_HIGH,d7
                cmpi.b  #CONTEXT_INDEX_FIVE,d4
                blt.b   .select_mid_compare_output
                cmpi.b  #CONTEXT_INDEX_SEVEN,d4
                ble.b   .compare_output
.select_mid_compare_output:
                move.w  #CONTEXT_OUTPUT_A_MID,d7
.compare_output:
                cmp.w   CONTEXT_OUTPUT_A.l,d7
                bne.b   .lookup_output
                move.w  #CONTEXT_OUTPUT_B_SPECIAL,CONTEXT_OUTPUT_B.l
                move.w  #CONTEXT_OUTPUT_C_SPECIAL,CONTEXT_OUTPUT_C.l
                bra.w   SHARED_COMMAND_QUEUE
.lookup_output:
                lea.l   CONTEXT_OUTPUT_LOOKUP_TABLE.l,a0
                move.b  CONTEXT_INDEX.l,d4
                ext.w   d4
                move.b  (a0,d4.w),d4
                ext.w   d4
                move.w  d4,CONTEXT_OUTPUT_B.l
                asl.w   #4,d4
                move.w  d4,CONTEXT_OUTPUT_C.l
                jsr     UPDATE_CONTEXT_OUTPUT.l
                move.b  CONTEXT_INDEX.l,d4
                cmpi.b  #CONTEXT_INDEX_THREE,d4
                blt.b   .store_low_output
                cmpi.b  #CONTEXT_INDEX_NINE,d4
                ble.b   .store_high_output
                cmpi.b  #CONTEXT_INDEX_TWELVE,d4
                bge.b   .store_high_output
.store_low_output:
                move.w  #CONTEXT_OUTPUT_A_LOW,CONTEXT_OUTPUT_A.l
                bra.w   SHARED_COMMAND_QUEUE
.store_high_output:
                cmpi.b  #CONTEXT_INDEX_FOUR,d4
                ble.b   .store_mid_output
                cmpi.b  #CONTEXT_INDEX_EIGHT,d4
                bge.b   .store_mid_output
                move.w  #CONTEXT_OUTPUT_A_HIGH,CONTEXT_OUTPUT_A.l
                bra.w   SHARED_COMMAND_QUEUE
.store_mid_output:
                move.w  #CONTEXT_OUTPUT_A_MID,CONTEXT_OUTPUT_A.l
                bra.w   SHARED_COMMAND_QUEUE
