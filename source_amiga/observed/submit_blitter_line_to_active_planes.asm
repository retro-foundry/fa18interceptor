; Byte-exact reconstruction of $C2FB7A-$C2FD21 (Hunk 36 +$6EA).
; Uses prepared line registers to trigger the blitter for each enabled plane.

                org     $C2FB7A

LINE_PLANE_POINTERS         equ $C456B6
ACTIVE_LINE_PLANE_MASK      equ $C456E7
LINE_PLANE_MODE             equ $C456E8
LINE_PLANE_FLAGS            equ $C456E9
LINE_CONTROL_FLAGS          equ $C45957
CUSTOM_DMACONR              equ $002
BLTCON0                     equ $040
BLTCON1                     equ $042
BLTCPT                      equ $048
BLTDPT                      equ $054
BLTAPTL                     equ $052
BLTBMOD                     equ $062
BLTSIZE                     equ $058
BLTADAT                     equ $074
BLITTER_BUSY_BIT            equ 6
PLANE_CONTROL_ADD_SET       equ $00FA
PLANE_CONTROL_ADD_CLEAR     equ $000A
BLIT_LINE_DATA              equ $8000

submit_blitter_line_to_active_planes:
                movea.l LINE_PLANE_POINTERS.l,a2
                movea.l d7,a1
                btst.b  #0,ACTIVE_LINE_PLANE_MASK.l
                beq.b   .plane_one
                move.w  d6,d5
                tst.w   LINE_PLANE_MODE.l
                blt.b   .plane_zero_negative_mode
                btst.b  #0,LINE_PLANE_FLAGS.l
                bra.b   .plane_zero_adjust_control
.plane_zero_negative_mode:
                btst.b  #0,LINE_CONTROL_FLAGS.l
.plane_zero_adjust_control:
                beq.b   .plane_zero_add_clear
                addi.w  #PLANE_CONTROL_ADD_SET,d5
                bra.b   .plane_zero_submit
.plane_zero_add_clear:
                addi.w  #PLANE_CONTROL_ADD_CLEAR,d5
.plane_zero_submit:
                move.l  $C(a2),d7
                add.l   a1,d7
.plane_zero_wait:
                btst.b  #BLITTER_BUSY_BIT,CUSTOM_DMACONR(a0)
                beq.b   .plane_zero_write
                nop
                nop
                bra.b   .plane_zero_wait
.plane_zero_write:
                move.w  d5,BLTCON0(a0)
                move.w  d1,BLTCON1(a0)
                move.w  d2,BLTAPTL(a0)
                move.l  d7,BLTCPT(a0)
                move.l  d7,BLTDPT(a0)
                move.w  #BLIT_LINE_DATA,BLTADAT(a0)
                move.w  d3,BLTBMOD(a0)
                move.w  d4,BLTSIZE(a0)
.plane_one:
                btst.b  #1,ACTIVE_LINE_PLANE_MASK.l
                beq.b   .plane_two
                move.w  d6,d5
                tst.w   LINE_PLANE_MODE.l
                blt.b   .plane_one_negative_mode
                btst.b  #1,LINE_PLANE_FLAGS.l
                bra.b   .plane_one_adjust_control
.plane_one_negative_mode:
                btst.b  #1,LINE_CONTROL_FLAGS.l
.plane_one_adjust_control:
                beq.b   .plane_one_add_clear
                addi.w  #PLANE_CONTROL_ADD_SET,d5
                bra.b   .plane_one_submit
.plane_one_add_clear:
                addi.w  #PLANE_CONTROL_ADD_CLEAR,d5
.plane_one_submit:
                move.l  $8(a2),d7
                add.l   a1,d7
.plane_one_wait:
                btst.b  #BLITTER_BUSY_BIT,CUSTOM_DMACONR(a0)
                beq.b   .plane_one_write
                nop
                nop
                bra.b   .plane_one_wait
.plane_one_write:
                move.w  d5,BLTCON0(a0)
                move.w  d1,BLTCON1(a0)
                move.w  d2,BLTAPTL(a0)
                move.l  d7,BLTCPT(a0)
                move.l  d7,BLTDPT(a0)
                move.w  #BLIT_LINE_DATA,BLTADAT(a0)
                move.w  d3,BLTBMOD(a0)
                move.w  d4,BLTSIZE(a0)
.plane_two:
                btst.b  #2,ACTIVE_LINE_PLANE_MASK.l
                beq.b   .plane_three
                move.w  d6,d5
                tst.w   LINE_PLANE_MODE.l
                blt.b   .plane_two_negative_mode
                btst.b  #2,LINE_PLANE_FLAGS.l
                bra.b   .plane_two_adjust_control
.plane_two_negative_mode:
                btst.b  #2,LINE_CONTROL_FLAGS.l
.plane_two_adjust_control:
                beq.b   .plane_two_add_clear
                addi.w  #PLANE_CONTROL_ADD_SET,d5
                bra.b   .plane_two_submit
.plane_two_add_clear:
                addi.w  #PLANE_CONTROL_ADD_CLEAR,d5
.plane_two_submit:
                move.l  $4(a2),d7
                add.l   a1,d7
.plane_two_wait:
                btst.b  #BLITTER_BUSY_BIT,CUSTOM_DMACONR(a0)
                beq.b   .plane_two_write
                nop
                nop
                bra.b   .plane_two_wait
.plane_two_write:
                move.w  d5,BLTCON0(a0)
                move.w  d1,BLTCON1(a0)
                move.w  d2,BLTAPTL(a0)
                move.l  d7,BLTCPT(a0)
                move.l  d7,BLTDPT(a0)
                move.w  #BLIT_LINE_DATA,BLTADAT(a0)
                move.w  d3,BLTBMOD(a0)
                move.w  d4,BLTSIZE(a0)
.plane_three:
                btst.b  #3,ACTIVE_LINE_PLANE_MASK.l
                beq.b   .return
                move.w  d6,d5
                tst.w   LINE_PLANE_MODE.l
                blt.b   .plane_three_negative_mode
                btst.b  #3,LINE_PLANE_FLAGS.l
                bra.b   .plane_three_adjust_control
.plane_three_negative_mode:
                btst.b  #3,LINE_CONTROL_FLAGS.l
.plane_three_adjust_control:
                beq.b   .plane_three_add_clear
                addi.w  #PLANE_CONTROL_ADD_SET,d5
                bra.b   .plane_three_submit
.plane_three_add_clear:
                addi.w  #PLANE_CONTROL_ADD_CLEAR,d5
.plane_three_submit:
                move.l  (a2),d7
                add.l   a1,d7
.plane_three_wait:
                btst.b  #BLITTER_BUSY_BIT,CUSTOM_DMACONR(a0)
                beq.b   .plane_three_write
                nop
                nop
                bra.b   .plane_three_wait
.plane_three_write:
                move.w  d5,BLTCON0(a0)
                move.w  d1,BLTCON1(a0)
                move.w  d2,BLTAPTL(a0)
                move.l  d7,BLTCPT(a0)
                move.l  d7,BLTDPT(a0)
                move.w  #BLIT_LINE_DATA,BLTADAT(a0)
                move.w  d3,BLTBMOD(a0)
                move.w  d4,BLTSIZE(a0)
.return:
                rts
