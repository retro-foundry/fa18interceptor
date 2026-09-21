; Byte-exact partially runtime-observed terminal postflight record scan $C3180C-$C318F5.

                org     $C3180C

POSTFLIGHT_SCAN_REQUEST          equ $C457B9
POSTFLIGHT_RECORD_TRIPLES        equ $C4E2BC
POSTFLIGHT_CURRENT_INDEX         equ $C459C0
POSTFLIGHT_SCAN_FALLBACK         equ $C4599E
POSTFLIGHT_RECORD_BASE           equ $C46184
POSTFLIGHT_RESULT_WORD           equ $C459C4
POSTFLIGHT_RESULT_BYTE           equ $C45886
POSTFLIGHT_TIMER_BYTE            equ $C45887
POSTFLIGHT_SLOT_BYTE             equ $C45868
POSTFLIGHT_MASK_WORD             equ $C458CE
POSTFLIGHT_EXTERNAL_FALLBACK     equ $C06C02

scan_postflight_records:
                tst.b   POSTFLIGHT_SCAN_REQUEST.l
                beq.w   postflight_scan_return
                clr.b   POSTFLIGHT_SCAN_REQUEST.l
                lea     POSTFLIGHT_RECORD_TRIPLES.l,a0
                moveq   #0,d0
                tst.w   POSTFLIGHT_CURRENT_INDEX.l
                blt.s   postflight_scan_entry
postflight_scan_find_current:
                move.w  $C(a0,d0.w),d1
                andi.w  #$FF00,d1
                add.w   d1,d1
                cmp.w   POSTFLIGHT_CURRENT_INDEX.l,d1
                beq.s   postflight_scan_advance
                addi.w  #$10,d0
                cmpi.w  #$460,d0
                blt.s   postflight_scan_find_current
                move.w  #$30,POSTFLIGHT_SCAN_FALLBACK.l
                jsr     POSTFLIGHT_EXTERNAL_FALLBACK.l
                moveq   #0,d0
                bra.s   postflight_scan_entry
postflight_scan_advance:
                addi.w  #$10,d0
postflight_scan_entry:
                move.l  (a0,d0.w),d5
                or.l    $4(a0,d0.w),d5
                or.l    $8(a0,d0.w),d5
                beq.s   postflight_scan_empty
                move.w  $C(a0,d0.w),d2
                btst    #4,d2
                beq.s   postflight_scan_next
                btst    #5,d2
                beq.s   postflight_scan_next
                lea     POSTFLIGHT_RECORD_BASE.l,a2
                andi.w  #$FF00,d2
                add.w   d2,d2
                move.b  $62(a2,d2.w),d5
                andi.b  #$F0,d5
                ; CMPI.B #0,D5. VASM otherwise substitutes TST.B D5.
                dc.w    $0C05,$0000
                beq.s   postflight_scan_next
                cmpi.b  #$20,d5
                beq.s   postflight_scan_next
                cmpi.b  #$30,d5
                beq.s   postflight_scan_next
                btst    #1,$20(a2,d2.w)
                bne.s   postflight_scan_next
                move.w  d2,POSTFLIGHT_CURRENT_INDEX.l
                move.w  #1,POSTFLIGHT_RESULT_WORD.l
                move.b  #1,POSTFLIGHT_RESULT_BYTE.l
                move.b  #-$1,POSTFLIGHT_TIMER_BYTE.l
                asr.w   #4,d0
                addq.b  #1,d0
                move.b  d0,POSTFLIGHT_SLOT_BYTE.l
                andi.w  #$DFFF,POSTFLIGHT_MASK_WORD.l
                bra.s   postflight_scan_return
postflight_scan_next:
                addi.w  #$10,d0
                bra.w   postflight_scan_entry
postflight_scan_empty:
                move.b  #0,POSTFLIGHT_SLOT_BYTE.l
                move.w  #-$1,POSTFLIGHT_CURRENT_INDEX.l
postflight_scan_return:
                rts
