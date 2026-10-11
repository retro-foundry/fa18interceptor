; Byte-exact keyboard command-dispatch entry and context gates $C1AD74-$C1AE27.

                org     $C1AD74

DISPATCH_FRAME_COUNTER          equ $C457D5
DISPATCH_STATE_A                equ $C45785
DISPATCH_STATE_B                equ $C45878
COMMAND_CONTEXT_STATE           equ $C458AE
COMMAND_CONTEXT_GATE            equ $C458AD
COMMAND_CONTEXT_MODE            equ $C458A6
COMMAND_STATE                   equ $C457D3
COMMAND_ENABLE_FLAG             equ $C45791
COMMAND_GATE_FLAG               equ $C45787
COMMAND_MODE_LATCH              equ $C4584B
COMMAND_BLOCK_FLAGS             equ $C46200
COMMAND_BLOCK_MASK              equ $0F
CONTEXT_STATE_THREE             equ 3
CONTEXT_STATE_ONE               equ 1
CONTEXT_STATE_TWO               equ 2
CONTEXT_STATE_FIVE              equ 5
FRAME_COUNTER_BIT               equ 7

DISPATCH_FRAME_COUNTER_WRAP     equ $C1AD72
DISPATCH_DIRECT_COMMAND_KEYS    equ $C1AE28
DISPATCH_ALTERNATE_CONTEXT      equ $C1AEE0
DISPATCH_FUNCTION_AND_INDEXED   equ $C1AFA2
DISPATCH_FALLBACK_COMMAND_KEYS  equ $C1B010
DISPATCH_CONTEXT_MODE_KEYS      equ $C1B030
DISPATCH_NONZERO_CONTEXT        equ $C1B038

gate_keyboard_command_dispatch:
                link.w  a6,#0
                move.l  8(a6),d0
                unlk    a6
                tst.b   DISPATCH_FRAME_COUNTER.l
                bgt.b   .load_context
                addq.b  #1,DISPATCH_FRAME_COUNTER.l
                ble.b   DISPATCH_FRAME_COUNTER_WRAP
                bclr    #FRAME_COUNTER_BIT,d0
.load_context:
                move.b  DISPATCH_STATE_A.l,d5
                move.b  DISPATCH_STATE_B.l,d6
                move.b  COMMAND_CONTEXT_STATE.l,d3
                cmpi.b  #CONTEXT_STATE_THREE,d3
                beq.w   DISPATCH_NONZERO_CONTEXT
                tst.b   COMMAND_STATE.l
                bne.w   DISPATCH_NONZERO_CONTEXT
                tst.b   COMMAND_CONTEXT_GATE.l
                bne.w   DISPATCH_NONZERO_CONTEXT
                tst.b   COMMAND_ENABLE_FLAG.l
                bne.w   DISPATCH_FUNCTION_AND_INDEXED
                tst.b   COMMAND_GATE_FLAG.l
                beq.w   DISPATCH_FUNCTION_AND_INDEXED
                tst.b   COMMAND_CONTEXT_MODE.l
                beq.w   DISPATCH_FUNCTION_AND_INDEXED
                move.b  COMMAND_MODE_LATCH.l,d1
                blt.w   DISPATCH_FUNCTION_AND_INDEXED
                cmpi.b  #CONTEXT_STATE_THREE,d1
                beq.w   DISPATCH_CONTEXT_MODE_KEYS
                tst.b   d3
                bne.w   DISPATCH_ALTERNATE_CONTEXT
                cmpi.b  #CONTEXT_STATE_TWO,COMMAND_CONTEXT_MODE.l
                beq.w   DISPATCH_ALTERNATE_CONTEXT
                move.b  COMMAND_BLOCK_FLAGS.l,d4
                andi.b  #COMMAND_BLOCK_MASK,d4
                bne.w   DISPATCH_ALTERNATE_CONTEXT
                cmpi.b  #CONTEXT_STATE_ONE,d1
                beq.w   DISPATCH_ALTERNATE_CONTEXT
                cmpi.b  #CONTEXT_STATE_TWO,d1
                beq.w   DISPATCH_ALTERNATE_CONTEXT
                cmpi.b  #CONTEXT_STATE_FIVE,d1
                bge.w   DISPATCH_CONTEXT_MODE_KEYS
