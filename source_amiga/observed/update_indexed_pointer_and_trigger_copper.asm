; Byte-exact structural indexed pointer/update stage $C17B2C-$C17B95.
; Table, record, argument, and display meanings remain unassigned.
                org     $C17B2C

SOURCE_INDEX_ARGUMENT           equ $08
TARGET_INDEX_ARGUMENT           equ $0C
SHIFTED_VALUE_ARGUMENT          equ $10
SOURCE_POINTER_TABLE            equ $C0A438
TARGET_POINTER_TABLE            equ $C4FE38
SOURCE_RECORD_LONG_OFFSET       equ $0C
CLEAR_AND_TRIGGER_TARGET        equ $C17B08
TRIGGER_INDEXED_COPPER_JUMP     equ $C4FFB0

update_indexed_pointer_and_trigger_copper:
                link    a6,#0
                movem.l d2,-(sp)
                move.l  SOURCE_INDEX_ARGUMENT(a6),d0
                asl.l   #2,d0
                movea.l d0,a0
                adda.l  #SOURCE_POINTER_TABLE,a0
                tst.l   (a0)
                beq.s   .done
                move.l  TARGET_INDEX_ARGUMENT(a6),-(sp)
                bsr.s   CLEAR_AND_TRIGGER_TARGET
                addq.l  #4,sp
                move.l  SOURCE_INDEX_ARGUMENT(a6),d0
                asl.l   #2,d0
                movea.l d0,a0
                adda.l  #SOURCE_POINTER_TABLE,a0
                movea.l (a0),a1
                moveq   #$10,d1
                move.l  SHIFTED_VALUE_ARGUMENT(a6),d2
                asl.l   d1,d2
                move.l  d2,SOURCE_RECORD_LONG_OFFSET(a1)
                move.l  TARGET_INDEX_ARGUMENT(a6),d1
                asl.l   #2,d1
                movea.l d1,a0
                adda.l  #TARGET_POINTER_TABLE,a0
                movea.l d0,a1
                adda.l  #SOURCE_POINTER_TABLE,a1
                move.l  (a1),(a0)
                move.l  TARGET_INDEX_ARGUMENT(a6),-(sp)
                jsr     TRIGGER_INDEXED_COPPER_JUMP.l
                addq.l  #4,sp
.done:
                movem.l (sp)+,d2
                unlk    a6
                rts
