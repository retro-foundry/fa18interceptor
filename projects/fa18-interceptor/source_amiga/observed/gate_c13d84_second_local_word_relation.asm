; Byte-exact observed C13D84 continuation $C14366-$C14375.
; This second relation gate compares the same signed local-word pair and
; chooses its corresponding high or nonpositive continuation route.

                org     $C14366

gate_c13d84_second_local_word_relation:
                move.w  -$1c(a6),d0
                cmp.w   -$16(a6),d0
                bge.b   $C143BE
                tst.w   -$18(a6)
                ble.b   $C1437E
