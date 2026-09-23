; Byte-exact control-record candidate selection $C29368-$C29408.
; A signed-word-terminated list at C4573A is scanned for a record whose byte
; at +$62 has high nibble $10 and whose +1 byte has bit 6 set.  The selected
; record supplies an origin candidate; meanings of the list and record type
; remain unassigned.

                org     $C29368

ORIGIN_AUXILIARY_FLAG     equ     $C458AF
ORIGIN_ADJUSTMENT_MODE    equ     $C457B6
ORIGIN_RECORD_LIST        equ     $C4573A
ORIGIN_SELECTED_OFFSET    equ     $C459BA
ORIGIN_FALLBACK_WORD      equ     $C4599E
ORIGIN_RECORD_BASE        equ     $C46184
ORIGIN_MIDDLE             equ     $C45C42
ORIGIN_CANDIDATE_TRIPLE   equ     $C45C56

select_terrain_origin_control_record:
                move.b  #1,ORIGIN_AUXILIARY_FLAG.l
                clr.w   ORIGIN_SELECTED_OFFSET.l
                move.b  #7,ORIGIN_ADJUSTMENT_MODE.l
                lea.l   ORIGIN_RECORD_BASE.l,a0
                movea.l ORIGIN_RECORD_LIST.l,a2
.scan_record_list:
                tst.w   (a2)
                blt.b   .no_matching_record
                move.w  $4(a2),d3
                dc.w    $D4FC,$000A       ; adda.w #$a,a2; preserve original encoding
                lsl.w   #8,d3
                add.w   d3,d3
                move.b  $62(a0,d3.w),d1
                andi.b  #$f0,d1
                cmpi.b  #$10,d1
                bne.b   .scan_record_list
                btst.b  #6,$1(a0,d3.w)
                beq.b   .scan_record_list
                move.w  d3,ORIGIN_SELECTED_OFFSET.l
                btst.b  #3,$1(a0,d3.w)
                beq.b   .publish_selected_record
                bra.b   .scan_record_list
.no_matching_record:
                tst.w   ORIGIN_SELECTED_OFFSET.l
                bne.b   .publish_selected_record
                move.w  #$3e,ORIGIN_FALLBACK_WORD.l
                jsr     $C06C02.l
                jsr     $C0910C.l
                bra.b   .publish_candidate
.publish_selected_record:
                move.w  ORIGIN_SELECTED_OFFSET.l,d3
                move.b  #$ff,ORIGIN_AUXILIARY_FLAG.l
                move.l  $14(a0,d3.w),d0
                move.l  $1c(a0,d3.w),d2
                move.l  ORIGIN_MIDDLE.l,d1
                move.l  d1,d3
                asr.l   #2,d3
                sub.l   d3,d1
.publish_candidate:
                movem.l d0-d2,ORIGIN_CANDIDATE_TRIPLE.l
                rts
