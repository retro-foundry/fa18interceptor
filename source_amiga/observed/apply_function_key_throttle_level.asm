; Byte-exact function-key throttle tail $C1BD04-$C1BD77.
; Reached by raw function-key values $50-$59.

                org     $C1BD04

FIRST_FUNCTION_KEY_RAW          equ $4F
FUNCTION_KEY_LEVEL_ONE           equ 1
FUNCTION_KEY_LEVEL_FIELD         equ $C45870
LEVEL_ONE_COMPARE_FIELD           equ $C461AF
LEVEL_ONE_SPECIAL_VALUE           equ $0C
FUNCTION_KEY_LEVEL_MAX           equ $78
THROTTLE_CONTROL_ACTIVE          equ $C457D8
THROTTLE_CONTROL_SCALE_SHIFT     equ 3
THROTTLE_CONTROL_MAX             equ $03C0
THROTTLE_CONTROL_WORD            equ $C45778
THROTTLE_CONTROL_COMPANION_WORD  equ $C4577C
SHARED_COMMAND_FALLBACK          equ $C1C23C

apply_function_key_throttle_level:
                move.w  d0,d4
                subi.w  #FIRST_FUNCTION_KEY_RAW,d4
                cmpi.w  #FUNCTION_KEY_LEVEL_ONE,d4
                bne.s   .store_scaled_level
                tst.b   FUNCTION_KEY_LEVEL_FIELD.l
                bne.s   .check_level_one_special_value
                cmpi.b  #LEVEL_ONE_SPECIAL_VALUE,LEVEL_ONE_COMPARE_FIELD.l
                beq.s   .set_level_one_special_value
.check_level_one_special_value:
                cmpi.b  #LEVEL_ONE_SPECIAL_VALUE,FUNCTION_KEY_LEVEL_FIELD.l
                bne.s   .store_scaled_level
.set_level_one_special_value:
                move.b  #$FF,FUNCTION_KEY_LEVEL_FIELD.l
                bra.w   SHARED_COMMAND_FALLBACK
.store_scaled_level:
                add.b   d4,d4
                move.b  d4,d5
                add.b   d4,d4
                add.b   d5,d4
                add.b   d4,d4
                cmpi.b  #FUNCTION_KEY_LEVEL_MAX,d4
                blt.s   .store_level
                addq.b  #1,d4
.store_level:
                move.b  d4,FUNCTION_KEY_LEVEL_FIELD.l
                tst.b   THROTTLE_CONTROL_ACTIVE.l
                beq.w   SHARED_COMMAND_FALLBACK
                ext.w   d4
                asl.w   #THROTTLE_CONTROL_SCALE_SHIFT,d4
                cmpi.w  #THROTTLE_CONTROL_MAX,d4
                ble.s   .publish_control_words
                move.w  #THROTTLE_CONTROL_MAX,d4
.publish_control_words:
                move.w  d4,THROTTLE_CONTROL_WORD.l
                move.w  d4,THROTTLE_CONTROL_COMPANION_WORD.l
                bra.w   SHARED_COMMAND_FALLBACK
