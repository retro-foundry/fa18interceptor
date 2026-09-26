; Byte-exact observed shifted matrix-auxiliary term gate $C122C0-$C122E7.
; Two cached longwords are arithmetic-shifted by three into local words, then
; the first is tested against the strict interval ($0258,$0C4E).

                org     $C122C0

MATRIX_AUXILIARY_CACHE          equ     $C45A88

gate_shifted_matrix_auxiliary_terms:
                move.l  MATRIX_AUXILIARY_CACHE.l,d0
                asr.l   #3,d0
                move.l  $C45A90.l,d1
                asr.l   #3,d1
                move.w  d0,-$2(a6)
                move.w  d1,-$4(a6)
                move.w  -$2(a6),d0
                cmpi.w  #$258,d0
                ble.b   $C12316
                cmpi.w  #$C4E,d0
                bge.b   $C12316
