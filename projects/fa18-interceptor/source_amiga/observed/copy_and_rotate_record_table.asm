; Byte-exact observed record-table copy/rotation stage $C1E436-$C1E4CF.
; It copies 24-byte records through C48390, performs an indexed reorder, and
; advances a byte countdown. The record contents and display meaning are open.

                org     $C1E436

COPY_RECORD_LOOP                equ     $C1E37A
COUNTDOWN_RESTART               equ     $C1E336
COPY_WORKSPACE                  equ     $C48390
COPY_INDEX_STREAM               equ     $C4E828
COUNTDOWN_RELOAD                equ     $C4585C
COUNTDOWN_BYTE                  equ     $C4585D
HELPER_WORKSPACE                equ     $C4E7A4
HELPER_SOURCE                   equ     $C4E778
HELPER_CONTINUE                 equ     $C1E4D2

copy_and_rotate_record_table:
                move.w  d1,(a3)+
                dc.w    $D2FC,$000C             ; adda.w #$000C,a1
                dbra    d0,COPY_RECORD_LOOP
                bsr.w   .prepare_copy_workspace
                lea     (a2),a1
                lea     COPY_WORKSPACE.l,a5
                lea     (a5),a4
                subq.w  #1,d7
                move.w  d7,d6
.copy_records:
                movem.l (a2)+,d0-d5
                movem.l d0-d5,(a5)
                dc.w    $DAFC,$0018             ; adda.w #$0018,a5
                dbra    d6,.copy_records
                lea     COPY_INDEX_STREAM.l,a3
.reorder_records:
                move.w  (a3)+,d4
                asl.w   #3,d4
                move.w  d4,d5
                add.w   d4,d4
                add.w   d5,d4
                movem.l (a4,d4.w),d0-d5
                movem.l d0-d5,(a1)
                dc.w    $D2FC,$0018             ; adda.w #$0018,a1
                dbra    d7,.reorder_records
                subq.b  #1,COUNTDOWN_BYTE.l
                blt.s   .reload_countdown
                tst.b   -$2c(a6)
                beq.s   .restore_and_return
                bra.w   COUNTDOWN_RESTART
.reload_countdown:
                move.b  COUNTDOWN_RELOAD.l,COUNTDOWN_BYTE.l
.restore_and_return:
                movem.l (sp)+,d0-d7/a0-a5
                rts

.prepare_copy_workspace:
                movem.l d0-d6/a0-a5,-(sp)
                lea     HELPER_WORKSPACE.l,a5
                movem.l HELPER_SOURCE.l,d0-d6/a1-a4
                movem.l d0-d6/a1-a4,(a5)
                lea     $84(a5),a4
                move.w  d7,d1
                subq.w  #1,d1
                lea     (a5),a0
                move.w  d1,d6
.scan_workspace:
                move.w  (a0)+,d0
                bge.s   HELPER_CONTINUE
                dbra    d6,.scan_workspace
