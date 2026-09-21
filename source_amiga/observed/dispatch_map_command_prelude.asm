; Byte-exact queue entries and map-command prelude $C1BF78-$C1BFBB.

                org     $C1BF78

RECORD_WORD_MASK                equ $FFBF
COMMAND_CONTEXT_STATE           equ $C458AE
COMMAND_REQUEST_FLAGS           equ $C4599C
COMMAND_REQUEST_BIT             equ 0
COMMAND_STATE_FLAG              equ $C457AD
CONTEXT_LONG_VALUE              equ $C45C42
MAP_CONTEXT_LONG_SAVED          equ $C45664

SHARED_COMMAND_QUEUE            equ $C1C23C
CONTEXT_SPECIAL_REQUEST_RETRY   equ $C1B684
MAP_COMMAND_HELPER              equ $C0F4A6

clear_indexed_record_word_flag:
                andi.w  #RECORD_WORD_MASK,(a0,d4.w)
                bra.w   SHARED_COMMAND_QUEUE

invoke_map_command_side_effect:
                jsr     $C083E2.l
                bra.w   SHARED_COMMAND_QUEUE

dispatch_map_command_prelude:
                tst.b   COMMAND_CONTEXT_STATE.l
                bne.w   SHARED_COMMAND_QUEUE
                bset.b  #COMMAND_REQUEST_BIT,COMMAND_REQUEST_FLAGS.l
                tst.b   COMMAND_STATE_FLAG.l
                beq.b   .initialize_map_state
                move.l  CONTEXT_LONG_VALUE.l,MAP_CONTEXT_LONG_SAVED.l
                bra.w   CONTEXT_SPECIAL_REQUEST_RETRY
.initialize_map_state:
                move.w  d0,-(a7)
                jsr     MAP_COMMAND_HELPER.l
