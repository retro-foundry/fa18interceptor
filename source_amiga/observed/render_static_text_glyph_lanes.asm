; Byte-exact static-text glyph lane renderer $C33058-$C330FD.
; It is renderer/dataflow reconstruction; screen-coordinate semantics are unresolved.

                org     $C33058

MESSAGE_RECORD_ATTRIBUTE         equ     $C457DC
MESSAGE_AUXILIARY_BYTE           equ     $C457DE
MESSAGE_LAYOUT_TRIPLET           equ     $C456FE
MESSAGE_EVENT_LAYOUT_ENTRY       equ     $C32EF6
STATIC_TEXT_MASK_UPDATE          equ     $C330FE

render_static_text_glyph_lanes:
                add.w   d4,d4
                adda.w  (a3,d4.w),a3
                move.l  a3,d4
                btst    #3,d6
                bne.b   .lane0_enabled
                move.w  #$B0A,d2
                bra.b   .lane0_mask_ready
.lane0_enabled:
                move.w  #$BFA,d2
.lane0_mask_ready:
                or.w    d3,d2
                move.l  (a5),d1
                add.l   d5,d1
                bsr.w   STATIC_TEXT_MASK_UPDATE

                move.l  4(a5),d1
                add.l   d5,d1
                btst    #2,d6
                bne.b   .lane1_enabled
                move.w  #$B0A,d2
                bra.b   .lane1_mask_ready
.lane1_enabled:
                move.w  #$BFA,d2
.lane1_mask_ready:
                or.w    d3,d2
                bsr.w   STATIC_TEXT_MASK_UPDATE

                move.l  8(a5),d1
                add.l   d5,d1
                btst    #1,d6
                bne.b   .lane2_enabled
                move.w  #$B0A,d2
                bra.b   .lane2_mask_ready
.lane2_enabled:
                move.w  #$BFA,d2
.lane2_mask_ready:
                or.w    d3,d2
                bsr.w   STATIC_TEXT_MASK_UPDATE

                move.l  12(a5),d1
                add.l   d5,d1
                btst    #0,d6
                bne.b   .lane3_enabled
                move.w  #$B0A,d2
                bra.b   .lane3_mask_ready
.lane3_enabled:
                move.w  #$BFA,d2
.lane3_mask_ready:
                or.w    d3,d2
                bsr.w   STATIC_TEXT_MASK_UPDATE

                ; BTST #0,$C457DC.L; retain original absolute encoding.
                dc.b    8,57,0,0,0,196,87,220
                bne.b   .attribute_set
                addq.w  #4,a1
                addq.w  #1,a2
                movem.l a1-a2/a4,MESSAGE_LAYOUT_TRIPLET.l
                ; BRA.W $C32EF6; retain original word-branch encoding.
                dc.w    $6000,$FE10
.attribute_set:
                tst.b   MESSAGE_AUXILIARY_BYTE.l
                bgt.b   .store_layout
                addq.w  #4,a1
                addq.w  #1,a2
.store_layout:
                movem.l a1-a2/a4,MESSAGE_LAYOUT_TRIPLET.l
                rts

