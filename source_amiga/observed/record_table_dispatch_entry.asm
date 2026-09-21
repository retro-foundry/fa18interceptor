; Byte-exact dispatcher entry slice $C1F910-$C1F94D (Hunk 10 +$BD8).
; A2 supplies signed control/selector words.  Branches to the enclosing record
; walker are deliberately external to this slice.

                org     $C1F910

RECORD_DISPATCH_TABLE           equ $C1FCE8
RECORD_DISPATCH_ERROR_SLOT      equ $C4599E
REPORT_RECORD_DISPATCH_ERROR    equ $C06C02
RECORD_WALKER_CONTROL           equ $C1F8EC
RECORD_WALKER_CONTINUE          equ $C1F90A
RECORD_WALKER_RESTART           equ $C1F7FA
NEXT_RECORD_CONTROL             equ $C1F94E
COUNTED_SELECTOR_BIT            equ $4000
SELECTOR_TABLE_MASK             equ $3FFF
RECORD_COUNT_LOCAL              equ -$6A
RECORD_STATUS_LOCAL             equ -$7C

record_table_dispatch_entry:
                move.w  (a2)+,d0
                bge.s   NEXT_RECORD_CONTROL
                cmpi.w  #$FFFF,d0
                bne.s   .test_counted_selector
                move.w  d0,RECORD_DISPATCH_ERROR_SLOT.l
                jsr     REPORT_RECORD_DISPATCH_ERROR.l
                bra.s   RECORD_WALKER_CONTROL

.test_counted_selector:
                move.w  d0,d1
                andi.w  #COUNTED_SELECTOR_BIT,d1
                beq.s   .resolve_selector
                addq.w  #1,RECORD_COUNT_LOCAL(a6)
.resolve_selector:
                andi.w  #SELECTOR_TABLE_MASK,d0
                lea     RECORD_DISPATCH_TABLE.l,a0
                movea.l (a0,d0.w),a0
                jsr     (a0)
                blt.w   RECORD_WALKER_RESTART
                or.w    d0,RECORD_STATUS_LOCAL(a6)
                bra.s   RECORD_WALKER_CONTINUE