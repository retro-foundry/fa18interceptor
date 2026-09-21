; Byte-exact static-only $C2ECC6-$C2ED6B continuation of C2EC9C.
; The observed C2EC9C entry rejected before reaching this projection/dispatch path.
                org $C2ECC6
PROJECTED_PAIR equ $C45958
PROJECT_LIMIT equ $C45984
project_matrix_product_tuple:
 muls.w #$a0,d0
 divs.w d2,d0
 addi.w #$a0,d0
 blt.b .clamp_x_low
 cmpi.w #$140,d0
 bge.b .clamp_x_high
.project_y:
 muls.w #$5a,d1
 divs.w d2,d1
 addi.w #$5a,d1
 blt.b .clamp_y_low
 cmpi.w #$b4,d1
 bge.b .clamp_y_high
.store_pair:
 subi.w #$13f,d0
 neg.w d0
 subi.w #$b3,d1
 neg.w d1
 addq.w #1,d1
 cmp.w PROJECT_LIMIT.l,d1
 bgt.b $C2EC82
 movem.w d0-d1,PROJECTED_PAIR.l
 tst.w d7
 blt.b .advance_negative_mode
 move.w #$30,d2
 asr.w d7,d2
 cmp.w -40(a6),d2
 bge.b .dispatch_f5f4
 move.w #$50,d2
 asr.w d7,d2
 cmp.w -40(a6),d2
 bge.b .dispatch_f60a
 jsr $C2F5F4.l
 bra.b .success
.dispatch_f60a:
 jsr $C2F60A.l
 bra.b .success
.dispatch_f5f4:
 jsr $C2F66E.l
 bra.b .success
.dispatch_negative_mode:
 bsr.w $C2F1C0
.success:
 moveq #1,d0
 rts
.clamp_x_low:
 clr.w d0
 bra.b .project_y
.clamp_y_low:
 clr.w d1
 bra.b .store_pair
.clamp_x_high:
 move.w #$13f,d0
 bra.b .project_y
.clamp_y_high:
 move.w #$b3,d1
 bra.b .store_pair
.advance_negative_mode:
 addq.w #1,d7
 bge.b $C2ED24
 addq.w #1,d7
 bge.b $C2ED2C
 addq.w #1,d7
 bge.b $C2ED34
 addq.w #1,d7
 bge.b $C2ED3C
 move.w d0,d0
 rts
