; Byte-exact context command-index selector family $C1B8F8-$C1B9CB.

                org     $C1B8F8

COMMAND_REQUEST_FLAGS_A         equ $C4599C
COMMAND_REQUEST_FLAGS_B         equ $C4599D
REQUEST_FLAG_A                  equ 0
REQUEST_FLAG_B                  equ 1
REQUEST_FLAG_C                  equ 2
REQUEST_FLAG_D                  equ 3
REQUEST_FLAG_E                  equ 4
REQUEST_FLAG_F                  equ 6
CONTEXT_INDEX                   equ $C457A7
CONTEXT_INDEX_CLEAR             equ $C457A8
CONTEXT_INDEX_CLEAR_VALUE       equ 0
CONTEXT_INDEX_LIMIT             equ $0B
REQUEST_LATCH                   equ $C45858
REQUEST_LATCH_DISABLED          equ $FF
CONTEXT_INDEX_ZERO              equ 0
CONTEXT_INDEX_SIX               equ 6
CONTEXT_INDEX_ELEVEN            equ $0B
CONTEXT_INDEX_TWELVE            equ $0C
CONTEXT_INDEX_THIRTEEN          equ $0D

SELECT_SLOT_ZERO                equ $C1B77C
SELECT_SLOT_FOUR                equ $C1B796
SELECT_SLOT_FIVE                equ $C1B7A6
SELECT_SLOT_SIX                 equ $C1B7AA
SELECT_SLOT_SEVEN               equ $C1B7AE
SELECT_SLOT_EIGHT               equ $C1B7B2
STORE_CONTEXT_COMMAND_INDEX     equ $C1B9CC
SET_CONTEXT_COMMAND_CONTINUATION equ $C1B9DA
RESET_CONTEXT_COMMAND_INDEX     equ $C1B906

select_context_command_index_zero:
                bset.b  #REQUEST_FLAG_E,COMMAND_REQUEST_FLAGS_A.l
                tst.b   d5
                bne.w   SELECT_SLOT_ZERO
                clr.b   d7
                move.b  #CONTEXT_INDEX_CLEAR_VALUE,CONTEXT_INDEX_CLEAR.l
                bra.w   STORE_CONTEXT_COMMAND_INDEX

select_context_command_index_six:
                bset.b  #REQUEST_FLAG_A,COMMAND_REQUEST_FLAGS_B.l
                tst.b   d5
                bne.w   SELECT_SLOT_FIVE
                moveq   #CONTEXT_INDEX_SIX,d7
                move.b  #CONTEXT_INDEX_CLEAR_VALUE,CONTEXT_INDEX_CLEAR.l
                bra.w   STORE_CONTEXT_COMMAND_INDEX

decrement_context_command_index:
                bset.b  #REQUEST_FLAG_C,COMMAND_REQUEST_FLAGS_B.l
                tst.b   d5
                bne.w   SELECT_SLOT_SEVEN
                move.b  #CONTEXT_INDEX_CLEAR_VALUE,CONTEXT_INDEX_CLEAR.l
                subq.b  #1,CONTEXT_INDEX.l
                blt.b   .wrap_to_eleven
                beq.b   RESET_CONTEXT_COMMAND_INDEX
                cmpi.b  #CONTEXT_INDEX_LIMIT,CONTEXT_INDEX.l
                bge.b   RESET_CONTEXT_COMMAND_INDEX
                bra.b   SET_CONTEXT_COMMAND_CONTINUATION
.wrap_to_eleven:
                moveq   #CONTEXT_INDEX_ELEVEN,d7
                bra.b   STORE_CONTEXT_COMMAND_INDEX

increment_context_command_index:
                bset.b  #REQUEST_FLAG_F,COMMAND_REQUEST_FLAGS_A.l
                tst.b   d5
                bne.w   SELECT_SLOT_FOUR
                move.b  #CONTEXT_INDEX_CLEAR_VALUE,CONTEXT_INDEX_CLEAR.l
                addq.b  #1,CONTEXT_INDEX.l
                cmpi.b  #CONTEXT_INDEX_LIMIT,CONTEXT_INDEX.l
                ble.b   SET_CONTEXT_COMMAND_CONTINUATION
                bra.w   RESET_CONTEXT_COMMAND_INDEX

select_context_command_index_twelve:
                bset.b  #REQUEST_FLAG_D,COMMAND_REQUEST_FLAGS_B.l
                tst.b   d5
                bne.w   SELECT_SLOT_EIGHT
                moveq   #CONTEXT_INDEX_TWELVE,d7
                move.b  #CONTEXT_INDEX_CLEAR_VALUE,CONTEXT_INDEX_CLEAR.l
                move.b  #REQUEST_LATCH_DISABLED,REQUEST_LATCH.l
                bra.b   STORE_CONTEXT_COMMAND_INDEX

select_context_command_index_thirteen:
                bset.b  #REQUEST_FLAG_B,COMMAND_REQUEST_FLAGS_B.l
                tst.b   d5
                bne.w   SELECT_SLOT_SIX
                moveq   #CONTEXT_INDEX_THIRTEEN,d7
                move.b  #CONTEXT_INDEX_CLEAR_VALUE,CONTEXT_INDEX_CLEAR.l
                move.b  #REQUEST_LATCH_DISABLED,REQUEST_LATCH.l
