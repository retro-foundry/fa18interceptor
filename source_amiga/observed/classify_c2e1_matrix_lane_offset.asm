; Byte-exact observed matrix-lane classification $C2E1B4-$C2E205.
; It derives a table value from swapped D0, then selects a signed offset using
; two shared longword guards.  The table and output roles remain structural.

                org     $C2E1B4

MATRIX_LANE_GUARD_A             equ     $C45BBA
MATRIX_LANE_GUARD_B             equ     $C45BC2

classify_c2e1_matrix_lane_offset:
                swap    d0
                add.w   d0,d0
                bge.b   $C2E1BC
                neg.w   d0
                move.w  (a1,d0.w),d1
                move.w  #$384,d2
                sub.w   d1,d2
                move.w  d2,d1
                bra.b   $C2E1CE
                move.w  (a1,d0.w),d1
                move.w  #$E10,d2
                sub.w   d1,d2
                tst.l   MATRIX_LANE_GUARD_A.l
                blt.b   $C2E1F2
                tst.l   MATRIX_LANE_GUARD_B.l
                bge.b   $C2E202
                move.w  d2,d1
                cmpi.w  #$708,d1
                ble.b   $C2E1FC
                move.w  #$1518,d2
                bra.b   $C2E200
                tst.l   MATRIX_LANE_GUARD_B.l
                bge.b   $C2E202
                move.w  d2,d1
                move.w  #$708,d2
                sub.w   d1,d2
                move.w  d2,d5
                bge.b   $C2E208
