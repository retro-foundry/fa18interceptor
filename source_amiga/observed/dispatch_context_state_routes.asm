; Byte-exact context-state command routes $C1C052-$C1C0E3.

                org     $C1C052

COMMAND_CONTEXT_MODE            equ $C458A6
COMMAND_CONTEXT_SPECIAL         equ $7D
COMMAND_CONTEXT_STATE           equ $C458AE
CONTEXT_STATE_BYTE              equ $C457AE
CONTEXT_NEGATIVE_TARGET         equ $C458B0
CONTEXT_STATE_FLAG              equ $C45790
COMMAND_MODE_LATCH              equ $C4584B
COMMAND_WORD                    equ $C458DA
COMMAND_WORD_LOW3_CLEAR         equ $FFF8
CONTEXT_POINTER                 equ $C4FDAC
CONTEXT_SPECIAL_WORD            equ $C46986
CONTEXT_SPECIAL_BIT             equ $1000
CONTEXT_NEGATIVE_VALUE          equ $FE
CONTEXT_POSITIVE_VALUE          equ 1
CONTEXT_DISABLED_VALUE          equ $FF
BYTE_FILL_VALUE                 equ $FF

COMMAND_SIDE_EFFECT             equ $C33186
UPDATE_CONTEXT_STATE            equ $C0F4A6
PREPARE_COMMAND_QUEUE_CONTEXT   equ $C1C214
DISPATCH_GUARDED_CONTEXT_HELPER equ $C1C122
SHARED_COMMAND_QUEUE            equ $C1C23C

dispatch_context_special_toggle:
                cmpi.b  #COMMAND_CONTEXT_SPECIAL,COMMAND_CONTEXT_MODE.l
                bne.b   .queue
                jsr     COMMAND_SIDE_EFFECT.l
                eori.w  #CONTEXT_SPECIAL_BIT,CONTEXT_SPECIAL_WORD.l
.queue:
                bra.w   SHARED_COMMAND_QUEUE

dispatch_context_state_route:
                tst.b   COMMAND_CONTEXT_STATE.l
                blt.w   SHARED_COMMAND_QUEUE
                tst.b   CONTEXT_STATE_BYTE.l
                beq.b   .update_context
                move.b  #CONTEXT_NEGATIVE_VALUE,CONTEXT_NEGATIVE_TARGET.l
                lea.l   CONTEXT_STATE_BYTE.l,a0
                bra.w   PREPARE_COMMAND_QUEUE_CONTEXT
.update_context:
                move.w  d0,-(a7)
                jsr     UPDATE_CONTEXT_STATE.l
                move.w  (a7)+,d0
                tst.b   COMMAND_MODE_LATCH.l
                bne.b   .disable_context
                move.l  CONTEXT_POINTER.l,d3
                ble.b   .disable_context
                tst.b   CONTEXT_STATE_FLAG.l
                beq.b   .disable_context
                movea.l d3,a0
                moveq   #BYTE_FILL_VALUE,d3
                move.b  d3,(a0)+
                move.b  d3,(a0)+
                move.b  d3,(a0)+
                move.b  d3,(a0)+
.disable_context:
                move.b  #CONTEXT_DISABLED_VALUE,CONTEXT_STATE_BYTE.l
                bra.w   SHARED_COMMAND_QUEUE

enable_context_state_route:
                move.b  #CONTEXT_POSITIVE_VALUE,CONTEXT_STATE_BYTE.l
                andi.w  #COMMAND_WORD_LOW3_CLEAR,COMMAND_WORD.l
                bra.w   SHARED_COMMAND_QUEUE

dispatch_flare_pre_gate:
                tst.b   d6
                bne.b   DISPATCH_GUARDED_CONTEXT_HELPER
