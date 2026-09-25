; Byte-exact observed message-sequence consumer prefix $C32CEE-$C32D1D.
; Run060 passes head selector 74 through its direct fall-through to $C32D1E.
; Divergent continuations remain separately reconstructed or raw.

                org     $C32CEE

MESSAGE_SEQUENCE_INHIBIT        equ $C45871
MESSAGE_SELECTOR_SEQUENCE       equ $C4574A
MESSAGE_SELECTOR_BYTE_CURSOR    equ $C457C6
CURRENT_SELECTOR_WORD           equ $C45772
MESSAGE_SEQUENCE_ACTIVE         equ $C457C3
RETURN_COMMAND_EVENT            equ $C32CEC
RETURN_EMPTY_SELECTOR           equ $C32CC4
CONTINUE_ACTIVE_SEQUENCE        equ $C32E1A

consume_message_sequence_selector_prefix:
                tst.b   MESSAGE_SEQUENCE_INHIBIT.l
                bne.b   RETURN_COMMAND_EVENT
                lea.l   MESSAGE_SELECTOR_SEQUENCE.l,a0
                move.b  MESSAGE_SELECTOR_BYTE_CURSOR.l,d1
                ext.w   d1
                move.w  (a0,d1.w),d0
                beq.b   RETURN_EMPTY_SELECTOR
                tst.b   d1
                bne.b   .selector_not_at_head
                move.w  d0,CURRENT_SELECTOR_WORD.l
.selector_not_at_head:
                tst.b   MESSAGE_SEQUENCE_ACTIVE.l
                bne.w   CONTINUE_ACTIVE_SEQUENCE
