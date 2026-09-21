; Byte-exact guarded command helper route $C1C122-$C1C171.

                org     $C1C122

COMMAND_CONTEXT_MODE            equ $C458A6
COMMAND_CONTEXT_MODE_SIX        equ 6
COMMAND_GUARD_WORD              equ $C458C2
GUARD_RECORD_A_FLAGS            equ $C46385
GUARD_RECORD_B_FLAGS            equ $C46585
GUARD_RECORD_C_FLAGS            equ $C46785
GUARD_RECORD_BIT                equ 6
COMMAND_BLOCK_FLAGS             equ $C46200
COMMAND_BLOCK_LOW_MASK          equ $0F
HELPER_ARGUMENT_A               equ $30
HELPER_ARGUMENT_B               equ $1C

INVOKE_GUARDED_CONTEXT_HELPER   equ $C17F8C
SHARED_COMMAND_QUEUE            equ $C1C23C

dispatch_guarded_context_helper:
                cmpi.b  #COMMAND_CONTEXT_MODE_SIX,COMMAND_CONTEXT_MODE.l
                bne.b   .queue
                tst.w   COMMAND_GUARD_WORD.l
                bne.b   .queue
                btst.b  #GUARD_RECORD_BIT,GUARD_RECORD_A_FLAGS.l
                beq.b   .invoke
                btst.b  #GUARD_RECORD_BIT,GUARD_RECORD_B_FLAGS.l
                beq.b   .invoke
                btst.b  #GUARD_RECORD_BIT,GUARD_RECORD_C_FLAGS.l
                bne.b   .queue
.invoke:
                ori.b   #COMMAND_BLOCK_LOW_MASK,COMMAND_BLOCK_FLAGS.l
                move.w  d0,-(a7)
                moveq   #HELPER_ARGUMENT_A,d0
                move.l  d0,-(a7)
                moveq   #HELPER_ARGUMENT_B,d0
                move.l  d0,-(a7)
                jsr     INVOKE_GUARDED_CONTEXT_HELPER.l
                addq.l  #8,a7
                move.w  (a7)+,d0
.queue:
                bra.w   SHARED_COMMAND_QUEUE
