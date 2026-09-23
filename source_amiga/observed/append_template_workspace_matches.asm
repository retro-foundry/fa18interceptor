; Byte-exact conditional workspace-marker appender $C1D520-$C1D5D7.
; Called from the static-template selector with D1/D2/D3 as match fields and
; A2 as the current mutable workspace cursor.  It scans two bounded record
; regions and appends only the observed marker/index/terminator triplets.

                org     $C1D520

TEMPLATE_APPEND_ENABLE     equ     $C45864
TEMPLATE_APPEND_STATUS     equ     $C45AD4
TEMPLATE_APPEND_ERROR      equ     $C4599E
TEMPLATE_RECORD_REGION_A   equ     $C46184
TEMPLATE_RECORD_REGION_B   equ     $C48184

append_template_workspace_matches:
                tst.b   d1
                blt.b   $C1D51E
                tst.b   TEMPLATE_APPEND_ENABLE.l
                beq.b   $C1D51E
                move.w  #$50,TEMPLATE_APPEND_STATUS.l
                move.l  d6,-(a7)
                lea.l   TEMPLATE_RECORD_REGION_A.l,a0
                moveq   #0,d6
.scan_first_region:
                btst.b  #6,$1(a0)
                beq.b   .next_first_record
                btst.b  #4,$1(a0)
                beq.b   .next_first_record
                cmp.w   $6(a0),d3
                bne.b   .next_first_record
                cmp.w   $8(a0),d2
                bne.b   .next_first_record
                cmp.b   $a(a0),d1
                bne.b   .next_first_record
                bclr.b  #4,$1(a0)
                move.b  #$10,(a2)+
                move.b  d6,(a2)+
                move.b  #$ff,(a2)
.next_first_record:
                dc.w    $D0FC,$0200       ; adda.w #$200,a0; preserve original immediate form
                addq.b  #1,d6
                cmpi.b  #$10,d6
                blt.b   .scan_first_region
                lea.l   TEMPLATE_RECORD_REGION_B.l,a0
                moveq   #0,d6
.scan_second_region:
                btst.b  #6,$1(a0)
                beq.b   .next_second_record
                btst.b  #4,$1(a0)
                beq.b   .next_second_record
                cmp.w   $6(a0),d3
                bne.b   .next_second_record
                cmp.w   $8(a0),d2
                bne.b   .next_second_record
                cmp.b   $a(a0),d1
                bne.b   .next_second_record
                bclr.b  #4,$1(a0)
                move.b  #$40,(a2)+
                move.b  d6,(a2)+
                move.b  #$ff,(a2)
.next_second_record:
                dc.w    $D0FC,$0020       ; adda.w #$20,a0; preserve original immediate form
                addq.b  #1,d6
                cmpi.b  #$10,d6
                blt.b   .scan_second_region
                move.l  (a7)+,d6
                rts

.append_error:
                move.w  #$e,TEMPLATE_APPEND_ERROR.l
                jsr     $C06C02.l
                bra.b   .append_error
