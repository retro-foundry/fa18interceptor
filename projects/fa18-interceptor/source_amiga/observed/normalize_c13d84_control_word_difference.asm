; Byte-exact observed C13D84 continuation $C13E10-$C13E1F.
; It subtracts the shared reference control word from D0, retains the signed
; difference in local -$28, and normalizes that local to its absolute value.

                org     $C13E10

CONTROL_REFERENCE_WORD         equ     $C45778

normalize_c13d84_control_word_difference:
                move.w  CONTROL_REFERENCE_WORD.l,d1
                sub.w   d1,d0
                move.w  d0,-$28(a6)
                tst.w   d0
                bpl.b   $C13E24
                neg.w   -$28(a6)
