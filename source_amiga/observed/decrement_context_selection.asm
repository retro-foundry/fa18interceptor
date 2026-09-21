; Byte-exact context selection decrement route $C1B7F0-$C1B88F.

                org     $C1B7F0

CONTEXT_SELECTION               equ $C45785
COMMAND_CONTEXT_STATE           equ $C458AE
COMMAND_STATE_FLAG              equ $C457AD
COMMAND_REQUEST_FLAGS           equ $C4599C
COMMAND_REQUEST_BIT             equ 3
CONTEXT_LONG_VALUE              equ $C45C42
CONTEXT_LONG_STEP               equ $02000000
CONTEXT_LONG_MIN                equ $01000000
CONTEXT_SELECTION_MAX           equ 4
CONTEXT_SELECTION_DISABLED      equ $FF
REQUEST_LATCH_A                 equ $C45858
REQUEST_LATCH_B                 equ $C458B0
REQUEST_LATCH_C                 equ $C45891
REQUEST_LATCH_DISABLED          equ $FF

SELECT_CONTEXT_SLOT_EIGHT       equ $C1B7B6
SHARED_COMMAND_QUEUE            equ $C1C23C

decrement_context_selection:
                tst.b   CONTEXT_SELECTION.l
                bne.b   .selection_nonzero
                move.b  CONTEXT_SELECTION.l,d4
                bne.b   SELECT_CONTEXT_SLOT_EIGHT
                bset.b  #COMMAND_REQUEST_BIT,COMMAND_REQUEST_FLAGS.l
                bra.b   .set_request_latch_c
.selection_nonzero:
                move.b  CONTEXT_SELECTION.l,d4
                beq.w   SHARED_COMMAND_QUEUE
                tst.b   COMMAND_CONTEXT_STATE.l
                bne.w   SHARED_COMMAND_QUEUE
                bset.b  #COMMAND_REQUEST_BIT,COMMAND_REQUEST_FLAGS.l
                tst.b   COMMAND_STATE_FLAG.l
                beq.b   .decrement_byte_selection
                move.l  CONTEXT_LONG_VALUE.l,d1
                subi.l  #CONTEXT_LONG_STEP,d1
                cmpi.l  #CONTEXT_LONG_MIN,d1
                bge.b   .store_long_value
                move.l  #CONTEXT_LONG_MIN,d1
.store_long_value:
                move.l  d1,CONTEXT_LONG_VALUE.l
                move.b  #REQUEST_LATCH_DISABLED,REQUEST_LATCH_A.l
                bra.w   SHARED_COMMAND_QUEUE
.decrement_byte_selection:
                subq.b  #1,d4
                bgt.b   .store_byte_selection
                moveq   #REQUEST_LATCH_DISABLED,d4
.store_byte_selection:
                move.b  d4,CONTEXT_SELECTION.l
                move.b  #REQUEST_LATCH_DISABLED,REQUEST_LATCH_A.l
                tst.b   REQUEST_LATCH_B.l
                bne.w   SHARED_COMMAND_QUEUE
                move.b  #REQUEST_LATCH_DISABLED,REQUEST_LATCH_B.l
                bra.w   SHARED_COMMAND_QUEUE
.set_request_latch_c:
                move.b  #REQUEST_LATCH_DISABLED,REQUEST_LATCH_C.l
                bra.w   SHARED_COMMAND_QUEUE
