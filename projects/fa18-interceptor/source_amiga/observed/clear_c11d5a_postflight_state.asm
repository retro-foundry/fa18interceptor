; Byte-exact observed basic block $C11D5A-$C11D79.

                org     $C11D5A

POSTFLIGHT_FLAGS                equ     $C458CC
POSTFLIGHT_COMPARE_PATH         equ     $C11D9A

clear_c11d5a_postflight_state:
                move.w  POSTFLIGHT_FLAGS.l,d0
                btst    #2,d0
                bne.b   .retain_state_word
                clr.w   -$4(a6)
.retain_state_word:
                move.w  POSTFLIGHT_FLAGS.l,d0
                andi.w  #$ff7f,d0
                move.w  d0,POSTFLIGHT_FLAGS.l
                bra.b   POSTFLIGHT_COMPARE_PATH
