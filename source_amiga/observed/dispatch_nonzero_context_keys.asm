; Byte-exact nonzero-context raw-key dispatch $C1B038-$C1B0F1.

                org     $C1B038

COMMAND_CONTEXT_MODE            equ $C458AD
COMMAND_CONTEXT_MODE_TWO        equ 2
RAW_KEY_CONTEXT_SPECIAL         equ $43
RAW_KEY_PAIR_A_PRESS            equ $CC
RAW_KEY_PAIR_A_RELEASE          equ $CD
RAW_KEY_PAIR_B_PRESS            equ $CF
RAW_KEY_PAIR_B_RELEASE          equ $CE
RAW_KEY_PAIR_C_PRESS            equ $B8
RAW_KEY_PAIR_C_RELEASE          equ $B9
RAW_KEY_PAIR_D_PRESS            equ $8C
RAW_KEY_PAIR_D_RELEASE          equ $8B
RAW_KEY_PAIR_E                  equ $C0
RAW_KEY_LATCH_A_PRESS           equ $60
RAW_KEY_LATCH_A_RELEASE         equ $61
RAW_KEY_LATCH_B_PRESS           equ $66
RAW_KEY_LATCH_B_RELEASE         equ $E6
RAW_KEY_LATCH_C_PRESS           equ $67
RAW_KEY_LATCH_C_RELEASE         equ $E7
RAW_KEY_CANCEL_A                equ $E0
RAW_KEY_CANCEL_B                equ $E1
INPUT_LATCH_A                   equ $C45878
INPUT_LATCH_B                   equ $C45879
INPUT_LATCH_C                   equ $C4587A
INPUT_LATCH_SET                 equ 1

DISPATCH_CONTEXT_SPECIAL        equ $C1B664
DISPATCH_PAIR_A                 equ $C1B504
DISPATCH_PAIR_B                 equ $C1B550
DISPATCH_PAIR_C                 equ $C1B59A
DISPATCH_PAIR_D                 equ $C1B5C0
DISPATCH_PAIR_E                 equ $C1B22C
FINISH_INPUT_EVENT              equ $C1C2B6

dispatch_nonzero_context_keys:
                cmpi.b  #COMMAND_CONTEXT_MODE_TWO,COMMAND_CONTEXT_MODE.l
                bne.b   .check_pair_a
                cmpi.b  #RAW_KEY_CONTEXT_SPECIAL,d0
                beq.w   DISPATCH_CONTEXT_SPECIAL
.check_pair_a:
                cmpi.b  #RAW_KEY_PAIR_A_PRESS,d0
                beq.w   DISPATCH_PAIR_A
                cmpi.b  #RAW_KEY_PAIR_A_RELEASE,d0
                beq.w   DISPATCH_PAIR_A
                cmpi.b  #RAW_KEY_PAIR_B_PRESS,d0
                beq.w   DISPATCH_PAIR_B
                cmpi.b  #RAW_KEY_PAIR_B_RELEASE,d0
                beq.w   DISPATCH_PAIR_B
                cmpi.b  #RAW_KEY_PAIR_C_PRESS,d0
                beq.w   DISPATCH_PAIR_C
                cmpi.b  #RAW_KEY_PAIR_C_RELEASE,d0
                beq.w   DISPATCH_PAIR_C
                cmpi.b  #RAW_KEY_PAIR_D_PRESS,d0
                beq.w   DISPATCH_PAIR_D
                cmpi.b  #RAW_KEY_PAIR_D_RELEASE,d0
                beq.w   DISPATCH_PAIR_D
                cmpi.b  #RAW_KEY_PAIR_E,d0
                beq.w   DISPATCH_PAIR_E
                cmpi.b  #RAW_KEY_LATCH_A_PRESS,d0
                beq.b   .set_latch_a
                cmpi.b  #RAW_KEY_LATCH_A_RELEASE,d0
                bne.b   .check_cancel_a
.set_latch_a:
                move.b  #INPUT_LATCH_SET,INPUT_LATCH_A.l
                bra.w   FINISH_INPUT_EVENT
.check_cancel_a:
                cmpi.b  #RAW_KEY_CANCEL_A,d0
                beq.w   FINISH_INPUT_EVENT
                cmpi.b  #RAW_KEY_CANCEL_B,d0
                beq.w   FINISH_INPUT_EVENT
                cmpi.b  #RAW_KEY_LATCH_B_PRESS,d0
                beq.b   .set_latch_b
                cmpi.b  #RAW_KEY_LATCH_B_RELEASE,d0
                bne.b   .check_latch_c
                bra.w   FINISH_INPUT_EVENT
.set_latch_b:
                move.b  #INPUT_LATCH_SET,INPUT_LATCH_B.l
                bra.w   FINISH_INPUT_EVENT
.check_latch_c:
                cmpi.b  #RAW_KEY_LATCH_C_PRESS,d0
                beq.b   .set_latch_c
                cmpi.b  #RAW_KEY_LATCH_C_RELEASE,d0
                bne.b   .next_dispatch
                bra.w   FINISH_INPUT_EVENT
.set_latch_c:
                move.b  #INPUT_LATCH_SET,INPUT_LATCH_C.l
                bra.w   FINISH_INPUT_EVENT
.next_dispatch:
