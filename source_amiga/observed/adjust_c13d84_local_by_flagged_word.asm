; Byte-exact observed C13D84 flagged-local adjustment $C1432C-$C14343.
; It subtracts the paired local from the current term and, when bit 11 in a
; referenced word is set, subtracts one arithmetic quarter of that result.

                org     $C1432C

adjust_c13d84_local_by_flagged_word:
                move.w  -$16(a6),d0
                sub.w   -$1A(a6),d0
                movea.l -$2C(a6),a0
                move.w  (a0),d1
                move.w  d0,-$16(a6)
                btst    #11,d1
                beq.b   $C1434A
