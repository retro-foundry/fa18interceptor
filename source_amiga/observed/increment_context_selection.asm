; Byte-exact context selection increment route $C1B890-$C1B8F7.

                org     $C1B890

COMMAND_REQUEST_FLAGS           equ $C4599D
COMMAND_REQUEST_BIT             equ 7
CONTEXT_SELECTION               equ $C45785
COMMAND_CONTEXT_STATE           equ $C458AE
COMMAND_STATE_FLAG              equ $C457AD
CONTEXT_LONG_VALUE              equ $C45C42
CONTEXT_LONG_STEP               equ $02000000
CONTEXT_LONG_MAX                equ $08000000
CONTEXT_SELECTION_MAX           equ 4
REQUEST_LATCH_A                 equ $C45858
REQUEST_LATCH_DISABLED          equ $FF

STORE_DECREMENTED_SELECTION     equ $C1B860
SHARED_COMMAND_QUEUE            equ $C1C23C

increment_context_selection:
                bset.b  #COMMAND_REQUEST_BIT,COMMAND_REQUEST_FLAGS.l
                move.b  CONTEXT_SELECTION.l,d4
                beq.w   SHARED_COMMAND_QUEUE
                tst.b   COMMAND_CONTEXT_STATE.l
                bne.w   SHARED_COMMAND_QUEUE
                tst.b   COMMAND_STATE_FLAG.l
                beq.b   .increment_byte_selection
                move.l  CONTEXT_LONG_VALUE.l,d1
                addi.l  #CONTEXT_LONG_STEP,d1
                cmpi.l  #CONTEXT_LONG_MAX,d1
                ble.b   .store_long_value
                move.l  #CONTEXT_LONG_MAX,d1
.store_long_value:
                move.l  d1,CONTEXT_LONG_VALUE.l
                move.b  #REQUEST_LATCH_DISABLED,REQUEST_LATCH_A.l
                bra.w   SHARED_COMMAND_QUEUE
.increment_byte_selection:
                addq.b  #1,d4
                beq.b   .wrap_byte_selection
                cmpi.b  #CONTEXT_SELECTION_MAX,d4
                blt.w   STORE_DECREMENTED_SELECTION
                moveq   #CONTEXT_SELECTION_MAX,d4
                bra.w   STORE_DECREMENTED_SELECTION
.wrap_byte_selection:
                moveq   #1,d4
                bra.w   STORE_DECREMENTED_SELECTION
