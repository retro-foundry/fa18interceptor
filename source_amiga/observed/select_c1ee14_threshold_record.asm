; Byte-exact $C1EE58-$C1EE83 stream-record threshold selector.
; Run041 C35568/C355A0 chooses different records as live D1/D3 vary.
                org $C1EE58
select_c1ee14_threshold_record:
 move.w d0,d2
 andi.w #$3fff,d0
 asr.w d3,d0
 cmp.w d1,d0
 bgt.b .above_limit
 andi.w #$4000,d2
 beq.b .load_relative
 addq.w #4,a2
 bra.b $C1EE54
.load_relative:
 move.w (a2),d0
 blt.b $C1EEA0
 movea.l -$2c(a6),a2
 adda.w d0,a2
 bra.b $C1EE54
.above_limit:
 andi.w #$4000,d2
 bne.b .load_selected
 addq.w #2,a2
.load_selected:
 move.w (a2)+,d0
