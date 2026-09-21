; Byte-exact bounded prefix $C2E758-$C2E7D3.
; Initializes an eight-entry iteration over the prepared $C4B390 records.
                org $C2E758
DISPLAY_WORKSPACE equ $C4B990
DISPLAY_RECORDS equ $C4B390
initialize_display_record_iteration:
 link a6,#-4
 clr.b -2(a6)
 lea DISPLAY_WORKSPACE.l,a0
 lea DISPLAY_RECORDS.l,a1
 clr.w d0
 move.w #7,8(a6)
.next_record:
 btst #0,d0
 beq.b .even_record
 move.w d0,d3
 addq.w #1,d3
 cmp.w 8(a6),d3
 ble.b .previous_record_ready
 clr.w d3
.previous_record_ready:
 move.w d0,d1
 subq.w #1,d1
 bra.b .neighbors_ready
.even_record:
 move.w d0,d3
 move.w d0,d1
 addq.w #2,d1
 cmp.w 8(a6),d1
 ble.b .neighbors_ready
 clr.w d1
.neighbors_ready:
 asl.w #4,d1
 move.w d0,d6
 asl.w #3,d6
 lea (a0,d6.w),a3
 tst.b -2(a6)
 beq.b .process_record
 clr.b -2(a6)
 bra.w $C2EA38
.process_record:
 asl.w #4,d3
 movem.w (a1,d3.w),d3-d5
 addq.w #1,d3
 addq.w #1,d4
 cmp.w d5,d3
 blt.b $C2E7FC
 move.w (a1,d1.w),d2
 move.w 4(a1,d1.w),d6
 cmp.w d2,d6
 ble.w $C2E9DC
 bsr.w $C2EA5A
