; Byte-exact static-text glyph control prefix $C33002-$C33057.
; Attribute and event semantics remain unresolved.

                org     $C33002

MESSAGE_RECORD_ATTRIBUTE         equ     $C457DC
MESSAGE_SEQUENCE_MODE            equ     $C457E0
MESSAGE_AUXILIARY_BYTE           equ     $C457DE
MESSAGE_EVENT_CONTROL            equ     $C457D7
MESSAGE_LAYOUT_TRIPLET           equ     $C456FE
MESSAGE_EVENT_LAYOUT_ENTRY       equ     $C32EF6
MESSAGE_GUARDED_HELPER           equ     $C3316A
STATIC_TEXT_GLYPH_RENDER         equ     $C33058

dispatch_static_text_glyph_control:
                subi.w  #$20,d4
                ; BTST #0,$C457DC.L; retain original absolute encoding.
                dc.b    8,57,0,0,0,196,87,220
                beq.b   STATIC_TEXT_GLYPH_RENDER
                tst.w   d4
                bne.b   .attribute_set
                cmpi.b  #2,MESSAGE_SEQUENCE_MODE.l
                beq.b   .attribute_set
                move.b  MESSAGE_RECORD_ATTRIBUTE.l,d0
                andi.b  #2,d0
                beq.b   STATIC_TEXT_GLYPH_RENDER
                addq.w  #4,a1
                addq.w  #1,a2
                movem.l a1-a2/a4,MESSAGE_LAYOUT_TRIPLET.l
                ; BRA.W $C32EF6; retain original word-branch encoding.
                dc.w    $6000,$FEBE
.attribute_set:
                tst.b   MESSAGE_AUXILIARY_BYTE.l
                beq.b   STATIC_TEXT_GLYPH_RENDER
                move.b  MESSAGE_EVENT_CONTROL.l,d1
                beq.b   .set_helper_mode
                subq.b  #1,d1
                bne.b   STATIC_TEXT_GLYPH_RENDER
                moveq   #2,d1
                bra.b   .call_helper
.set_helper_mode:
                moveq   #2,d1
.call_helper:
                bsr.w   MESSAGE_GUARDED_HELPER
