; Byte-exact $C0D7E0-$C0D871 selector prefix for display record writes.
; Selects D0/D1 from four workspace words or branches to rejection.
                org $C0D7E0
DISPLAY_SELECTOR_WORKSPACE equ $C4E854
select_display_record_pair:
 lea DISPLAY_SELECTOR_WORKSPACE.l,a1
 tst.w 2(a1)
 beq.b .second_group
 tst.w 6(a1)
 beq.b .first_group_second_choice
 move.w 10(a1),d0
 move.w 14(a1),d1
 bra.w $C0D872
.first_group_second_choice:
 dc.w $4a69,$0000 ; tst.w 0(a1); retain original non-relaxed displacement
 beq.b .first_group_third_choice
 move.w 10(a1),d0
 move.w 8(a1),d1
 bra.w $C0D8BE
.first_group_third_choice:
 tst.w 4(a1)
 beq.b .reject_first_group
 move.w 10(a1),d0
 move.w 12(a1),d1
 bra.w $C0D90C
.reject_first_group:
 bra.w $C0DA94
.second_group:
 dc.w $4a69,$0000 ; tst.w 0(a1); retain original non-relaxed displacement
 beq.b .third_group
 tst.w 6(a1)
 beq.b .second_group_second_choice
 move.w 8(a1),d0
 move.w 14(a1),d1
 bra.w $C0D95A
.second_group_second_choice:
 tst.w 4(a1)
 beq.b .reject_second_group
 move.w 8(a1),d0
 move.w 12(a1),d1
 bra.w $C0D9A8
.reject_second_group:
 bra.w $C0DA94
.third_group:
 tst.w 4(a1)
 beq.w $C0DA38
 tst.w 6(a1)
 beq.b .reject_third_group
 move.w 12(a1),d0
 move.w 14(a1),d1
 bra.w $C0D9EA
.reject_third_group:
 bra.w $C0DA94
