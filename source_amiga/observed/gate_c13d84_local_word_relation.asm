; Byte-exact observed C13D84 continuation $C14226-$C14235.
; It compares signed locals -$1C and -$16, branching to the shared high
; route when the first is greater than or equal to the second.

                org     $C14226

gate_c13d84_local_word_relation:
                move.w  -$1c(a6),d0
                cmp.w   -$16(a6),d0
                bge.b   $C14284
                tst.w   -$18(a6)
                ble.b   $C14240
