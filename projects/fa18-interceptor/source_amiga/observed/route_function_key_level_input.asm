; Byte-exact function-key range gate $C1BC50-$C1BD03.
; Joins apply_function_key_throttle_level at $C1BD04.

                org     $C1BC50

LAST_FUNCTION_KEY_RAW           equ $59
FIRST_FUNCTION_KEY_RAW          equ $50
FUNCTION_KEY_RANGE_SIZE          equ 10
NON_FUNCTION_KEY_LEVEL_BASE      equ 1
FUNCTION_KEY_SECONDARY_LIMIT     equ 8
FUNCTION_KEY_ROUTE_MODE           equ $C458A6
FUNCTION_KEY_ROUTE_INPUT_FLAG     equ $C45879
FUNCTION_KEY_CONTEXT_SELECTOR     equ $C4584B
FUNCTION_KEY_AUXILIARY_FLAG       equ $C45791
FUNCTION_KEY_AUXILIARY_CODE       equ $C45849
FUNCTION_KEY_PENDING_INDEX        equ $C45787
FUNCTION_KEY_ROUTE_FLAG           equ $C457AE
FUNCTION_KEY_CONTEXT_FD           equ $FD
FUNCTION_KEY_AUXILIARY_ZERO_CODE  equ $10
FUNCTION_KEY_AUXILIARY_ONE_CODE   equ $11
FUNCTION_KEY_CONTEXT_FD_ADJUSTMENT equ 11
SHARED_COMMAND_FALLBACK           equ $C1C23C
FUNCTION_KEY_ALT_ROUTE            equ $C1BD78
FUNCTION_KEY_CONTEXT_FD_ROUTE     equ $C1BEDA

route_function_key_level_input:
                cmpi.w  #LAST_FUNCTION_KEY_RAW,d0
                bgt.s   .select_context_route
                tst.b   FUNCTION_KEY_ROUTE_MODE.l
                beq.s   .derive_function_key_level
                tst.b   FUNCTION_KEY_ROUTE_INPUT_FLAG.l
                beq.s   .select_context_route
.derive_function_key_level:
                move.w  d0,d4
                subi.w  #FIRST_FUNCTION_KEY_RAW,d4
                addi.w  #FUNCTION_KEY_RANGE_SIZE,d4
                bra.s   .check_auxiliary_route
.derive_non_function_key_level:
                move.w  d0,d4
                ; Preserve the original immediate-subtract encoding; vasm
                ; otherwise shortens this level-one operation to subq.w.
                dc.w    $0444,NON_FUNCTION_KEY_LEVEL_BASE ; subi.w #1,d4
.check_auxiliary_route:
                tst.b   FUNCTION_KEY_AUXILIARY_FLAG.l
                beq.s   .check_pending_index
                dc.w    $0C44,0                 ; cmpi.w #0,d4
                blt.s   .finish_auxiliary_route
                cmpi.w  #1,d4
                bgt.s   .finish_auxiliary_route
                beq.s   .set_auxiliary_one_code
                move.b  #FUNCTION_KEY_AUXILIARY_ONE_CODE,d4
                bra.s   .store_auxiliary_code
.set_auxiliary_one_code:
                move.b  #FUNCTION_KEY_AUXILIARY_ZERO_CODE,d4
.store_auxiliary_code:
                move.b  d4,FUNCTION_KEY_AUXILIARY_CODE.l
                clr.b   FUNCTION_KEY_AUXILIARY_FLAG.l
.finish_auxiliary_route:
                bra.w   SHARED_COMMAND_FALLBACK
.check_pending_index:
                tst.b   FUNCTION_KEY_PENDING_INDEX.l
                bne.w   FUNCTION_KEY_ALT_ROUTE
                dc.w    $0C44,0                 ; cmpi.w #0,d4
                blt.w   SHARED_COMMAND_FALLBACK
                cmpi.w  #FUNCTION_KEY_SECONDARY_LIMIT,d4
                bgt.w   SHARED_COMMAND_FALLBACK
                addq.w  #1,d4
                move.b  d4,FUNCTION_KEY_PENDING_INDEX.l
                bra.w   SHARED_COMMAND_FALLBACK
.select_context_route:
                move.b  FUNCTION_KEY_CONTEXT_SELECTOR.l,d2
                bgt.s   .return_to_shared_fallback
                tst.b   d6
                beq.s   .check_context_selector
                bra.s   .check_context_selector
.return_to_shared_fallback:
                bra.w   SHARED_COMMAND_FALLBACK
.check_context_selector:
                cmpi.b  #FUNCTION_KEY_CONTEXT_FD,d2
                beq.s   .adjust_fd_context_level
                tst.b   d2
                beq.s   .check_function_key_route_flag
                bra.w   SHARED_COMMAND_FALLBACK
.adjust_fd_context_level:
                subi.w  #FIRST_FUNCTION_KEY_RAW,d4
                addi.w  #FUNCTION_KEY_CONTEXT_FD_ADJUSTMENT,d4
                bra.w   FUNCTION_KEY_CONTEXT_FD_ROUTE
.check_function_key_route_flag:
                tst.b   FUNCTION_KEY_ROUTE_FLAG.l
                bne.w   SHARED_COMMAND_FALLBACK
