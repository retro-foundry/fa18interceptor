; Byte-exact leaf $C0DAA0-$C0DACF, called from the bounded $C0D752 packet.
; A1 is a caller-selected output cursor; semantic record ownership is unknown.
                org $C0DAA0
SCREEN_VALUES equ $C4B990
write_screen_relative_pair:
 asl.w #3,d0
 asl.w #3,d1
 lea SCREEN_VALUES.l,a0
 move.w #$13f,d2
 move.w d2,d4
 sub.w 0(a0,d0.w),d2
 move.w #$b3,d3
 move.w d3,d5
 sub.w 2(a0,d0.w),d3
 move.w d2,(a1)+
 move.w d3,(a1)+
 sub.w 0(a0,d1.w),d4
 sub.w 2(a0,d1.w),d5
 move.w d4,(a1)+
 move.w d5,(a1)+
 rts
