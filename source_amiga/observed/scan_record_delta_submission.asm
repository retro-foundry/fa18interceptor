; Byte-exact structural record-delta scan and submission stage $C1CFD6-$C1D0A3.
; Runtime evidence confirms execution; record ownership and C25876 semantics remain unassigned.
                org $C1CFD6
RECORD_SCAN_LATCH              equ $C45835
RECORD_SCAN_MODE               equ $C45838
RECORD_SCAN_ACTIVITY           equ $C45837
RECORD_SCAN_SELECTOR           equ $C458DA
RECORD_SCAN_SELECTED_OFFSET    equ $C458DE
RECORD_SCAN_RESULT_OFFSET      equ $C459AA
RECORD_SCAN_BASE               equ $C46184

scan_record_delta_submission:
                clr.b RECORD_SCAN_LATCH.l
                clr.b RECORD_SCAN_MODE.l
                tst.b $C45785.l
                bne.w .return
                cmpi.b #1,RECORD_SCAN_ACTIVITY.l
                bgt.b .set_mode
                move.w RECORD_SCAN_SELECTOR.l,d0
                move.w d0,d1
                andi.w #2,d1
                bne.b .check_selector
                addq.b #1,RECORD_SCAN_MODE.l
.check_selector:
                andi.w #3,d0
                bne.w .return
                bra.b .initialize
.set_mode:
                addq.b #1,RECORD_SCAN_MODE.l
.initialize:
                clr.w RECORD_SCAN_RESULT_OFFSET.l
                lea RECORD_SCAN_BASE.l,a1
                moveq #0,d0
.next_record:
                move.w RECORD_SCAN_SELECTED_OFFSET.l,d1
                cmp.w d1,d0
                beq.b .advance
                btst #6,$1(a1,d0.w)
                beq.b .advance
                movem.l $14(a1,d0.w),d2-d4
                sub.l $14(a1,d1.w),d2
                sub.l $18(a1,d1.w),d3
                sub.l $1c(a1,d1.w),d4
                move.l d2,d5
                bge.b .absolute_y
                neg.l d5
.absolute_y:
                move.l d3,d6
                bge.b .absolute_z
                neg.l d6
.absolute_z:
                move.l d4,d7
                bge.b .select_largest_component
                neg.l d7
.select_largest_component:
                cmp.l d5,d6
                blt.b .compare_x_z
                cmp.l d6,d7
                bgt.b .reduce_component
                move.l d6,d7
                bra.b .reduce_component
.compare_x_z:
                cmp.l d5,d7
                bgt.b .reduce_component
                move.l d5,d7
.reduce_component:
                moveq #0,d1
.reduce_until_bounded:
                cmpi.l #$7fffff,d7
                ble.b .shift_components
                addq.w #2,d1
                asr.l #2,d7
                bra.b .reduce_until_bounded
.shift_components:
                move.w d1,d5
                addq.w #8,d5
                asr.l d5,d2
                asr.l d5,d3
                asr.l d5,d4
                move.w d0,d7
                asr.w #1,d7
                bset #4,d7
                jsr $C25876
.advance:
                addi.w #$200,d0
                cmpi.w #$2000,d0
                blt.b .next_record
.return:
                rts
