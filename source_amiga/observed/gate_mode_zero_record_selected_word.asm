; Byte-exact observed mode-zero record-motion continuation $C24006-$C24013.
; It loads a shared selected word, tests the $FFFF sentinel, and returns to
; the preceding mode-zero path when that sentinel is present.

                org     $C24006

MODE_ZERO_SELECTED_WORD        equ     $C459C0

gate_mode_zero_record_selected_word:
                move.w  MODE_ZERO_SELECTED_WORD.l,d0
                cmpi.w  #-$1,d0
                beq.w   $C23F8E
