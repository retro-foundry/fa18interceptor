; Byte-exact static-text glyph setup $C32FCE-$C32FF1.
; Run029 key-6 trace reads the 'L' in SELECTABLE MISSIONS through this entry.

                org     $C32FCE

MESSAGE_LAYOUT_INDEX            equ $C45952
RESUME_GLYPH_COMPOSITOR         equ $C33002
RETURN_MESSAGE_SEQUENCE_ADVANCE equ $C32BD2
RETURN_MESSAGE_IDLE             equ $C32C04

prepare_static_text_glyph:
                move.b  (a2),d4
                blt.w   RETURN_MESSAGE_SEQUENCE_ADVANCE
                beq.w   RETURN_MESSAGE_IDLE
                move.w  (a1),d5
                ext.l   d5
                add.l   a4,d5
                move.w  2(a1),d3
                move.w  MESSAGE_LAYOUT_INDEX.l,d6
                andi.w  #$FF,d4
                cmpi.b  #8,d4
                bne.b   RESUME_GLYPH_COMPOSITOR
