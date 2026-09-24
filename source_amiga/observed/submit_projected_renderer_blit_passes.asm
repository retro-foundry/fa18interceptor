; Byte-exact C2F3AA-C2F47D four-pass custom-chip blit submission packet.
                org     $C2F3AA
RENDERER_WINDOW_LOOP             equ     $C2F2EC

submit_projected_renderer_blit_passes:
                moveq   #-$1,d0
                move.w  d0,$46(a2)
                lsr.w   -4(a6)
                bcc.b   renderer_blit_first_standard
                move.w  #$3FA,d0
                bra.b   renderer_blit_first_submit
renderer_blit_first_standard:
                move.w  #$30A,d0
renderer_blit_first_submit:
                move.w  d0,$40(a2)
                move.l  a0,$48(a2)
                move.l  a0,$54(a2)
                move.w  d7,$58(a2)
                lea     $1F40(a0),a0
                lsr.w   -4(a6)
                bcc.b   renderer_blit_second_standard
                move.w  #$3FA,d0
                bra.b   renderer_blit_second_wait
renderer_blit_second_standard:
                move.w  #$30A,d0
renderer_blit_second_wait:
                btst.b  #6,2(a2)
                beq.b   renderer_blit_second_submit
                nop
                nop
                bra.b   renderer_blit_second_wait
renderer_blit_second_submit:
                move.w  d0,$40(a2)
                move.l  a0,$48(a2)
                move.l  a0,$54(a2)
                move.w  d7,$58(a2)
                lea     $1F40(a0),a0
                lsr.w   -4(a6)
                bcc.b   renderer_blit_third_standard
                move.w  #$3FA,d0
                bra.b   renderer_blit_third_wait
renderer_blit_third_standard:
                move.w  #$30A,d0
renderer_blit_third_wait:
                btst.b  #6,2(a2)
                beq.b   renderer_blit_third_submit
                nop
                nop
                bra.b   renderer_blit_third_wait
renderer_blit_third_submit:
                move.w  d0,$40(a2)
                move.l  a0,$48(a2)
                move.l  a0,$54(a2)
                move.w  d7,$58(a2)
                lea     $1F40(a0),a0
                lsr.w   -4(a6)
                bcc.b   renderer_blit_fourth_standard
                move.w  #$3FA,d0
                bra.b   renderer_blit_fourth_wait
renderer_blit_fourth_standard:
                move.w  #$30A,d0
renderer_blit_fourth_wait:
                btst.b  #6,2(a2)
                beq.b   renderer_blit_fourth_submit
                nop
                nop
                bra.b   renderer_blit_fourth_wait
renderer_blit_fourth_submit:
                move.w  d0,$40(a2)
                move.l  a0,$48(a2)
                move.l  a0,$54(a2)
                move.w  d7,$58(a2)
                ; VASM canonicalizes this immediate ADDA as LEA; retain the
                ; captured word-size opcode rather than the equivalent LEA.
                dc.w    $D2FC,$0028              ; adda.w #$28,a1
                tst.w   d4
                blt.b   renderer_blit_next_row
                addq.w  #4,a4
                subq.w  #1,d4
                bge.w   RENDERER_WINDOW_LOOP
renderer_blit_next_row:
                subq.w  #4,a4
                subq.w  #1,d6
                bge.w   RENDERER_WINDOW_LOOP
