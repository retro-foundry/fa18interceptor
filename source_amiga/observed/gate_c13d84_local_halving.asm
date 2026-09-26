; Byte-exact observed C13D84 continuation $C1443C-$C14449.
; A byte-mask test on the record selected by local -$10 determines whether
; the signed local -$16 follows its halving continuation.

                org     $C1443C

gate_c13d84_local_halving:
                movea.l -$10(a6),a0
                move.b  (a0),d0
                andi.b  #$44,d0
                tst.b   d0
                bne.b   $C14456
