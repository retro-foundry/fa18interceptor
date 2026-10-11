; Byte-exact context and indexed raw-key dispatch $C1AF7C-$C1B00F.

                org     $C1AF7C

RAW_KEY_E                       equ $45
RAW_KEY_INDEX_FIRST              equ $01
RAW_KEY_INDEX_LAST               equ $0A
RAW_KEY_FUNCTION_FIRST          equ $50
RAW_KEY_FUNCTION_LAST           equ $59
RAW_KEY_ONE                     equ $1D
RAW_KEY_TWO                     equ $1E
RAW_KEY_THREE                   equ $1F
RAW_KEY_X                       equ $2D
RAW_KEY_C                       equ $2E
RAW_KEY_V                       equ $2F
RAW_KEY_Z                       equ $3D
RAW_KEY_LEFT                    equ $3E
COMMAND_CONTEXT_MODE            equ $C458A6
COMMAND_CONTEXT_MODE_TWO        equ 2
COMMAND_CONTEXT_STATE           equ $C458AE
CONTEXT_STATE_FIVE              equ 5
CONTEXT_STATE_SIX               equ 6

SET_INPUT_STATE_SIGN_FLAG       equ $C1C224
DISPATCH_INDEXED_KEY            equ $C1BC78
DISPATCH_LOW_INDEXED_KEY        equ $C1BC72
DISPATCH_ALTERNATE_MODE         equ $C1B010
DISPATCH_CONTEXT_GREATER_ZERO   equ $C1B038
ROUTE_FUNCTION_KEY_LEVEL        equ $C1BC50

dispatch_function_and_indexed_keys:
                cmpi.b  #RAW_KEY_E,d0
                beq.w   SET_INPUT_STATE_SIGN_FLAG
                cmpi.b  #COMMAND_CONTEXT_MODE_TWO,COMMAND_CONTEXT_MODE.l
                beq.w   DISPATCH_ALTERNATE_MODE
                cmpi.b  #CONTEXT_STATE_FIVE,d3
                beq.b   .function_and_indexed_keys
                cmpi.b  #CONTEXT_STATE_SIX,d3
                beq.b   .function_and_indexed_keys
                tst.b   d3
                bgt.w   DISPATCH_CONTEXT_GREATER_ZERO
.function_and_indexed_keys:
                cmpi.b  #RAW_KEY_INDEX_FIRST,d0
                blt.b   .indexed_keys
                cmpi.b  #RAW_KEY_INDEX_LAST,d0
                ble.w   DISPATCH_LOW_INDEXED_KEY
                cmpi.b  #RAW_KEY_FUNCTION_FIRST,d0
                blt.b   .indexed_keys
                cmpi.b  #RAW_KEY_FUNCTION_LAST,d0
                ble.w   ROUTE_FUNCTION_KEY_LEVEL
.indexed_keys:
                cmpi.b  #RAW_KEY_ONE,d0
                bne.b   .check_two
                moveq   #0,d4
                bra.b   .dispatch_index
.check_two:
                cmpi.b  #RAW_KEY_TWO,d0
                bne.b   .check_three
                moveq   #1,d4
                bra.b   .dispatch_index
.check_three:
                cmpi.b  #RAW_KEY_THREE,d0
                bne.b   .check_x
                moveq   #2,d4
                bra.b   .dispatch_index
.check_x:
                cmpi.b  #RAW_KEY_X,d0
                bne.b   .check_c
                moveq   #3,d4
                bra.b   .dispatch_index
.check_c:
                cmpi.b  #RAW_KEY_C,d0
                bne.b   .check_v
                moveq   #4,d4
                bra.b   .dispatch_index
.check_v:
                cmpi.b  #RAW_KEY_V,d0
                bne.b   .check_z
                moveq   #5,d4
                bra.b   .dispatch_index
.check_z:
                cmpi.b  #RAW_KEY_Z,d0
                bne.b   .check_left
                moveq   #6,d4
                bra.b   .dispatch_index
.check_left:
                cmpi.b  #RAW_KEY_LEFT,d0
                bne.b   DISPATCH_ALTERNATE_MODE
                moveq   #7,d4
.dispatch_index:
                bra.w   DISPATCH_INDEXED_KEY
