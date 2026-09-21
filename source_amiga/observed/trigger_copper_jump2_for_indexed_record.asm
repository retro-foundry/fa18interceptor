; Byte-exact structural OCS Copper-jump leaf $C4FFB0-$C4FFC9.
; The table's record ownership and +$14 field meaning remain unassigned.
                org     $C4FFB0

STACK_INDEX_ARGUMENT            equ $04
INDEXED_RECORD_TABLE            equ $C4FE28
RECORD_COPJMP2_WRITE_OFFSET     equ $14
COPJMP2                         equ $DFF09C

trigger_copper_jump2_for_indexed_record:
                move.l  STACK_INDEX_ARGUMENT(sp),d0
                asl.l   #2,d0
                lea.l   INDEXED_RECORD_TABLE.l,a0
                movea.l 0(a0,d0.w),a0
                move.w  RECORD_COPJMP2_WRITE_OFFSET(a0),COPJMP2.l
                rts
