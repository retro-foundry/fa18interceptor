; Byte-exact observed basic block $C1205C-$C12081.
; The unequal-byte route beginning at $C12082 is unobserved in the captures.

                org     $C1205C

POSTFLIGHT_STATE_WORD           equ     $C45AE0
POSTFLIGHT_STATE_COPY           equ     $C45ADE
POSTFLIGHT_DISPLAY_FLAGS        equ     $C45862
POSTFLIGHT_DISPLAY_STATE        equ     $C45860

clear_c1205c_postflight_display_flags:
                move.w  POSTFLIGHT_STATE_WORD.l,POSTFLIGHT_STATE_COPY.l
                move.b  POSTFLIGHT_DISPLAY_FLAGS.l,d0
                andi.b  #$c0,d0
                move.b  d0,POSTFLIGHT_DISPLAY_FLAGS.l
                move.b  POSTFLIGHT_DISPLAY_STATE.l,d1
                cmp.b   d0,d1
                beq.b   $C1208E
