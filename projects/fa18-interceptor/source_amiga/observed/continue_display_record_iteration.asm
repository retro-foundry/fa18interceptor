; Byte-exact $C2E9D8-$C2E9F7 shared failure/continuation tail of C2E758.
                org $C2E9D8
continue_display_record_iteration:
 tst.w d5
 bge.b .defer_or_advance
.clear_workspace_and_record:
 clr.l (a3)
 move.w d0,d6
 asl.w #4,d6
 clr.w 14(a1,d6.w)
 bra.w $C2EA38
.defer_or_advance:
 btst #0,d0
 beq.b $C2EA38
 move.b #1,-2(a6)
 bra.b $C2EA38
