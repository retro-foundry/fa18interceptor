; Byte-exact static template-stream selector $C1D3F4-$C1D51D.
; D0 selects a signed relative group entry from the caller-provided table at
; -4(A6); D1 is searched in that group's sorted word list.  A5 receives the
; selected immutable stream for the C1D442 workspace-copy loop.  The writes
; through A2 remain mutable workspace output, not source geometry.

                org     $C1D3F4

TEMPLATE_SELECTOR_DIRECTORY equ    -$4
TEMPLATE_BITSET_BASE        equ    -$8
TEMPLATE_STATUS_WORD        equ    $C4599E
TEMPLATE_WORKSPACE_LIMIT    equ    $C45A2E

select_static_template_stream:
                movem.l d2-d5/a0-a5,-(a7)
                move.w  d0,d2
                move.w  d1,d3
                movea.l TEMPLATE_SELECTOR_DIRECTORY(a6),a0
                add.w   d0,d0
                adda.w  (a0,d0.w),a0
                tst.w   (a0)
                blt.w   .finish
                asl.w   #3,d0
                move.w  d1,d6
                move.w  d1,d4
                asr.w   #5,d6
                add.w   d6,d6
                add.w   d6,d6
                add.w   d0,d6
                andi.w  #$1f,d4
                movea.l TEMPLATE_BITSET_BASE(a6),a5
                move.l  (a5,d6.w),d5
                btst.l  d4,d5
                beq.w   .finish
                lea.l   (a0),a5
                adda.w  (a0),a5
                addq.l  #2,a5
                bsr.w   search_static_template_row
                add.w   d0,d0
                add.w   d0,d0
                movea.l (a5,d0.w),a5
                move.b  #$ff,d1
.next_stream_item:
                move.b  (a5)+,d0
                cmpi.b  #$ff,d0
                beq.w   .stream_end
                move.b  d0,d6
                andi.b  #$f,d0
                cmp.b   d0,d1
                beq.b   .reuse_workspace_block
                bsr.w   $C1D520
                moveq   #$10,d5
                move.b  d0,d1
                ext.w   d0
                asl.w   #5,d0
                move.w  d0,d7
                add.w   d0,d0
                add.w   d7,d0
                lea.l   (a1,d0.w),a2
                move.l  a2,TEMPLATE_WORKSPACE_LIMIT.l
                addi.l  #$5d,TEMPLATE_WORKSPACE_LIMIT.l
.reuse_workspace_block:
                subq.b  #1,d5
                blt.b   .workspace_full
                cmpa.l  TEMPLATE_WORKSPACE_LIMIT.l,a2
                bge.b   .stream_end
                move.b  (a5)+,d7
                move.b  d7,d0
                andi.b  #$80,d7
                move.b  d7,(a2)+
                andi.b  #$7f,d0
                move.b  d0,(a2)+
                btst    #7,d7
                beq.b   .copy_template_words
                lsr.b   #4,d6
                andi.w  #$f,d6
                add.w   d6,d6
                lea.l   $C1D8B6.l,a3
                move.b  (a3,d6.w),d0
                ext.w   d0
                move.w  d0,(a2)+
                move.b  $1(a3,d6.w),d0
                move.w  d0,(a2)+
                bra.b   .terminate_workspace_item
.copy_template_words:
                move.l  (a5)+,(a2)+
.terminate_workspace_item:
                move.b  #$ff,(a2)
                bra.w   .next_stream_item
.workspace_full:
                move.w  #$38,TEMPLATE_STATUS_WORD.l
                jsr     $C06C02.l
                bra.b   .workspace_full
.stream_end:
                bsr.w   $C1D520
.finish:
                bsr.w   $C1D5D8
                movem.l (a7)+,d2-d5/a0-a5
                rts

; A0 begins with an even byte count, followed by sorted words.  On success D0
; is the matching word index, used by the caller to index the following long
; pointer table through A5.
search_static_template_row:
                moveq   #0,d6
                move.w  (a0)+,d7
                asr.w   #1,d7
.next_interval:
                move.w  d7,d0
                sub.w   d6,d0
                blt.b   .not_found
                asr.w   #1,d0
                add.w   d6,d0
                move.w  d0,d4
                add.w   d4,d4
                cmp.w   (a0,d4.w),d1
                beq.b   .found
                blt.b   .lower_half
                move.w  d0,d6
                addq.w  #1,d6
                bra.b   .next_interval
.lower_half:
                move.w  d0,d7
                subq.w  #1,d7
                bra.b   .next_interval
.found:
                rts
.not_found:
                move.w  #$1c,TEMPLATE_STATUS_WORD.l
                jsr     $C06C02.l
                rts
