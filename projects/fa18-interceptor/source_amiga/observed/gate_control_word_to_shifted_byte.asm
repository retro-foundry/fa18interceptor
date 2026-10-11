; Byte-exact observed control-word-to-byte gate $C140BA-$C140D5.
; For local-record bit 3, the signed control word is capped at $360; smaller
; values continue to the adjacent right-shift-and-store path.

                org     $C140BA

CONTROL_STATE_WORD              equ     $C45778

gate_control_word_to_shifted_byte:
                movea.l -$30(a6),a0
                move.w  (a0),d0
                btst    #3,d0
                beq.b   $C140E0
                move.w  CONTROL_STATE_WORD,d0
                ext.l   d0
                cmpi.l  #$360,d0
                bge.b   $C14114
