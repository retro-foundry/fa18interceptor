; Byte-exact context-special request entry $C1B664-$C1B6C1.

                org     $C1B664

COMMAND_ENTRY_MODE              equ $C457A3
COMMAND_ENTRY_MODE_VALUE        equ 2
COMMAND_REQUEST_FLAGS           equ $C4599C
COMMAND_REQUEST_BIT_ZERO        equ 1
COMMAND_STATE_D5                equ $C458B2
COMMAND_STATE_D5_VALUE          equ 4
COMMAND_STATE_A                 equ $C457B5
COMMAND_STATE_B                 equ $C457B4
COMMAND_STATE_SET               equ 1
COMMAND_STATE_CLEAR             equ $C457AD
COMMAND_RECORD_BASE             equ $C46184
COMMAND_RECORD_INDEX            equ $C458DE
COMMAND_RECORD_WORD_OFFSET      equ $68
COMMAND_RECORD_WORD             equ $C4592A
COMMAND_WORD_FLAGS              equ $C458C6
COMMAND_WORD_FLAG               equ 2
COMMAND_STATE_DISABLED          equ $C45833
COMMAND_STATE_DISABLED_VALUE    equ $FF

SHARED_COMMAND_QUEUE            equ $C1C23C
DISPATCH_CONTEXT_SPECIAL_CALC   equ $C1B6C2

dispatch_context_special_request:
                move.b  #COMMAND_ENTRY_MODE_VALUE,COMMAND_ENTRY_MODE.l
                tst.b   d6
                bne.b   DISPATCH_CONTEXT_SPECIAL_CALC
                bset.b  #COMMAND_REQUEST_BIT_ZERO,COMMAND_REQUEST_FLAGS.l
                tst.b   d5
                bne.b   .set_shared_state
                move.b  #COMMAND_STATE_D5_VALUE,COMMAND_STATE_D5.l
.set_shared_state:
                move.b  #COMMAND_STATE_SET,COMMAND_STATE_A.l
                move.b  #COMMAND_STATE_SET,COMMAND_STATE_B.l
                clr.b   COMMAND_STATE_CLEAR.l
                lea.l   COMMAND_RECORD_BASE.l,a0
                move.w  COMMAND_RECORD_INDEX.l,d4
                move.w  COMMAND_RECORD_WORD_OFFSET(a0,d4.w),COMMAND_RECORD_WORD.l
                ori.w   #COMMAND_WORD_FLAG,COMMAND_WORD_FLAGS.l
                move.b  #COMMAND_STATE_DISABLED_VALUE,COMMAND_STATE_DISABLED.l
                bra.w   SHARED_COMMAND_QUEUE
