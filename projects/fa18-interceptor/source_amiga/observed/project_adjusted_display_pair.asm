; Byte-exact $C2E9F8-$C2EA59 projection and loop tail of $C2E758.
; Converts the accepted adjusted triplet at $C45AC6 into a clamped pair.
                org $C2E9F8
ADJUSTED_PAIR_SNAPSHOT equ $C45AC6
project_adjusted_display_pair:
 movem.w ADJUSTED_PAIR_SNAPSHOT.l,d3-d5
 tst.w d5
.wait_positive_divisor:
 ble.b .wait_positive_divisor
 muls.w #$140,d3
 divs.w d5,d3
 asr.w #1,d3
 bcc.b .x_unrounded
 addq.w #1,d3
.x_unrounded:
 addi.w #$a0,d3
 blt.b .clamp_x_low
 cmpi.w #$140,d3
 bge.b .clamp_x_high
.project_y:
 muls.w #$b4,d4
 divs.w d5,d4
 asr.w #1,d4
 bcc.b .y_unrounded
 addq.w #1,d4
.y_unrounded:
 addi.w #$5a,d4
 blt.b .clamp_y_low
 cmpi.w #$b4,d4
 bge.b .clamp_y_high
.store_pair:
 movem.w d3-d4,(a3)
.next_record:
 addq.w #1,d0
 cmp.w 8(a6),d0
 ble.w $C2E774
 unlk a6
 rts
.clamp_x_low:
 clr.w d3
 bra.b .project_y
.clamp_y_low:
 clr.w d4
 bra.b .store_pair
.clamp_x_high:
 move.w #$13f,d3
 bra.b .project_y
.clamp_y_high:
 move.w #$b3,d4
 bra.b .store_pair
