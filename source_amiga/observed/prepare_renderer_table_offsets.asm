; Byte-exact four-plane pixel address/mask prefix $C2F688-$C2F6D7.
; Selects a one-hot 16-pixel word mask and, for in-range X, the
; word-wrapped 40*y + 2*floor(x/16) byte offset.
; The later output/dispatch phase writes Chip RAM; this prefix does not.
                org $C2F688
RENDER_MODE_WORD equ $C45954
prepare_renderer_table_offsets:
 tst.w d1
 ble.b $C2F622
 move.w RENDER_MODE_WORD.l,d2
 andi.w #$f,d2
 add.w d2,d2
 add.w d2,d2
 movea.l (a4,d2.w),a4
 move.w d0,d2
 andi.w #$f,d0
 add.w d0,d0
 move.w (a3,d0.w),d0
 move.w d0,d7
 not.w d0
 asl.w #3,d1
 move.w d1,d3
 add.w d3,d3
 add.w d3,d3
 add.w d3,d1
 andi.w #$fff0,d2
 asr.w #3,d2
 add.w d1,d2
 movem.l (a1),a0-a3
 adda.w d2,a0
 adda.w d2,a1
 adda.w d2,a2
 adda.w d2,a3
 move.w d0,d1
 move.w d0,d2
 move.w d0,d3
 move.w d7,d4
 move.w d7,d5
 move.w d7,d6
