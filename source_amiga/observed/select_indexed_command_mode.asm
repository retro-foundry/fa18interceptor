; Byte-exact indexed command-mode selector $C1BD78-$C1BE5F.

                org     $C1BD78

COMMAND_CONTEXT_MODE            equ $C458A6
COMMAND_MODE_STATE              equ $C45792
COMMAND_MODE_STATE_ONE          equ 1
COMMAND_MODE_STATE_TWO          equ 2
COMMAND_MODE_RECORD_POINTER     equ $C1AB74
COMMAND_MODE_RECORD_ENABLE      equ $C4FDBC
COMMAND_MODE_RECORD_INDEX       equ $06
COMMAND_MODE_VALIDATION_OFFSET  equ $12
COMMAND_MODE_RANGE_SUBTRACT     equ $0A
COMMAND_MODE_RANGE_MAX          equ 5
COMMAND_MODE_MIN                equ 3
COMMAND_MODE_MAX                equ 8
COMMAND_MODE_SPECIAL_THREE      equ $7D
COMMAND_MODE_SPECIAL_FOUR       equ 9
COMMAND_MODE_SPECIAL_FIVE       equ $FF
COMMAND_MODE_SPECIAL_SEVEN      equ $FE
COMMAND_MODE_SPECIAL_ZERO       equ $7F

APPLY_COMMAND_MODE_SIDE_EFFECT  equ $C3318E
SHARED_COMMAND_QUEUE            equ $C1C23C
SELECT_ALTERNATE_COMMAND_MODE   equ $C1BE60

select_indexed_command_mode:
                tst.b   COMMAND_CONTEXT_MODE.l
                bne.w   SELECT_ALTERNATE_COMMAND_MODE
                ; Captured encoding is cmpi.b #0,d4, not VASM's tst.b optimization.
                dc.w    $0C04,0
                blt.w   SHARED_COMMAND_QUEUE
                tst.b   d4
                beq.w   .select_zero_mode
                cmpi.b  #1,d4
                beq.b   .select_one_mode
                cmpi.b  #2,d4
                beq.b   .store_mode
                cmpi.b  #3,d4
                beq.w   .select_special_three
                cmpi.b  #4,d4
                beq.w   .select_special_four
                cmpi.b  #5,d4
                beq.w   .select_special_five
                cmpi.b  #7,d4
                beq.w   .select_special_seven
                movea.l COMMAND_MODE_RECORD_POINTER.l,a0
                ; Captured encoding retains the explicit zero displacement.
                dc.w    $4A68,0
                bne.b   .validate_remaining_mode
                bra.b   .apply_mode_side_effect
.validate_remaining_mode:
                cmpi.b  #6,d4
                bne.b   .validate_offset_mode
                move.b  #COMMAND_MODE_STATE_ONE,COMMAND_MODE_STATE.l
                move.b  COMMAND_MODE_RECORD_INDEX(a0),d4
                addq.b  #1,d4
                cmpi.b  #COMMAND_MODE_MIN,d4
                blt.b   .clamp_to_min
                cmpi.b  #COMMAND_MODE_MAX,d4
                ble.b   .store_mode
.clamp_to_min:
                moveq   #COMMAND_MODE_MIN,d4
.store_mode:
                move.b  d4,COMMAND_CONTEXT_MODE.l
.apply_mode_side_effect:
                jsr     APPLY_COMMAND_MODE_SIDE_EFFECT.l
                bra.w   SHARED_COMMAND_QUEUE
.select_one_mode:
                tst.b   d6
                beq.b   .store_mode
                move.b  #COMMAND_MODE_STATE_TWO,COMMAND_MODE_STATE.l
                bra.b   .store_mode
.validate_offset_mode:
                subi.b  #COMMAND_MODE_RANGE_SUBTRACT,d4
                ; Captured encoding is cmpi.b #0,d4, not VASM's tst.b optimization.
                dc.w    $0C04,0
                blt.w   SHARED_COMMAND_QUEUE
                cmpi.b  #COMMAND_MODE_RANGE_MAX,d4
                bgt.b   .queue
                addq.b  #1,d4
                addq.b  #2,d4
                cmpi.b  #COMMAND_MODE_MIN,d4
                beq.b   .store_mode
                lea.l   COMMAND_MODE_VALIDATION_OFFSET(a0),a0
                ext.w   d4
                tst.b   -1(a0,d4.w)
                beq.w   SHARED_COMMAND_QUEUE
                bra.b   .store_mode
.queue:
                bra.w   SHARED_COMMAND_QUEUE
.select_special_seven:
                moveq   #COMMAND_MODE_SPECIAL_SEVEN,d4
                bra.b   .store_mode
.select_special_five:
                moveq   #COMMAND_MODE_SPECIAL_FIVE,d4
                bra.b   .store_mode
.select_special_three:
                moveq   #COMMAND_MODE_SPECIAL_THREE,d4
                bra.b   .store_mode
.select_special_four:
                moveq   #COMMAND_MODE_SPECIAL_FOUR,d4
                bra.b   .store_mode
.select_zero_mode:
                move.b  #COMMAND_MODE_STATE_ONE,COMMAND_MODE_STATE.l
                tst.l   COMMAND_MODE_RECORD_ENABLE.l
                beq.w   SHARED_COMMAND_QUEUE
                moveq   #COMMAND_MODE_SPECIAL_ZERO,d4
                bra.b   .store_mode
