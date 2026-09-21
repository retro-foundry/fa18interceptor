; Byte-exact context request-slot selectors $C1B77C-$C1B7EF.

                org     $C1B77C

COMMAND_REQUEST_FLAGS_A         equ $C4599C
COMMAND_REQUEST_FLAGS_B         equ $C4599D
REQUEST_FLAG_A                  equ 3
REQUEST_FLAG_B                  equ 4
REQUEST_FLAG_C                  equ 5
REQUEST_FLAG_D                  equ 7
REQUEST_SLOT                     equ $C458B2
REQUEST_SLOT_ZERO               equ 0
REQUEST_SLOT_ONE                equ 1
REQUEST_SLOT_TWO                equ 2
REQUEST_SLOT_THREE              equ 3
REQUEST_SLOT_FOUR               equ 4
REQUEST_SLOT_FIVE               equ 5
REQUEST_SLOT_SIX                equ 6
REQUEST_SLOT_SEVEN              equ 7
REQUEST_SLOT_EIGHT              equ 8
REQUEST_SLOT_NINE               equ 9
REQUEST_LATCH_A                 equ $C45858
REQUEST_LATCH_B                 equ $C458B0
REQUEST_LATCH_DISABLED          equ $FF
REQUEST_CONTEXT_FLAG            equ $C45785

SHARED_COMMAND_QUEUE            equ $C1C23C
SELECTOR_ONE_CONTEXT            equ $C1B906

select_context_request_slot_zero:
                moveq   #REQUEST_SLOT_ZERO,d4
                bra.b   publish_context_request_slot

select_context_request_slot_one:
                bset.b  #REQUEST_FLAG_C,COMMAND_REQUEST_FLAGS_A.l
                tst.b   REQUEST_CONTEXT_FLAG.l
                beq.w   SELECTOR_ONE_CONTEXT
                moveq   #REQUEST_SLOT_ONE,d4
                bra.b   publish_context_request_slot

select_context_request_slot_two:
                moveq   #REQUEST_SLOT_TWO,d4
                bra.b   publish_context_request_slot

select_context_request_slot_three:
                bset.b  #REQUEST_FLAG_D,COMMAND_REQUEST_FLAGS_A.l
                moveq   #REQUEST_SLOT_THREE,d4
                bra.b   publish_context_request_slot

select_context_request_slot_four:
                moveq   #REQUEST_SLOT_FOUR,d4
                bra.b   publish_context_request_slot

select_context_request_slot_five:
                moveq   #REQUEST_SLOT_FIVE,d4
                bra.b   publish_context_request_slot

select_context_request_slot_six:
                moveq   #REQUEST_SLOT_SIX,d4
                bra.b   publish_context_request_slot

select_context_request_slot_seven:
                moveq   #REQUEST_SLOT_SEVEN,d4
                bra.b   publish_context_request_slot

select_context_request_slot_eight:
                bset.b  #REQUEST_FLAG_B,COMMAND_REQUEST_FLAGS_B.l
                moveq   #REQUEST_SLOT_EIGHT,d4
                bra.b   publish_context_request_slot

select_context_request_slot_nine:
                bset.b  #REQUEST_FLAG_C,COMMAND_REQUEST_FLAGS_B.l
                moveq   #REQUEST_SLOT_NINE,d4
publish_context_request_slot:
                move.b  d4,REQUEST_SLOT.l
                move.b  #REQUEST_LATCH_DISABLED,REQUEST_LATCH_A.l
                tst.b   REQUEST_LATCH_B.l
                bne.w   SHARED_COMMAND_QUEUE
                move.b  #REQUEST_LATCH_DISABLED,REQUEST_LATCH_B.l
                bra.w   SHARED_COMMAND_QUEUE
