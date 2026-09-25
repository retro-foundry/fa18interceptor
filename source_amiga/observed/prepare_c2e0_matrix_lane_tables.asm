; Byte-exact observed matrix-lane table preparation $C2E07C-$C2E0C9.
; It rounds a swapped input, derives paired table values, and loads an indexed
; secondary value.  Table semantics remain structural.

                org     $C2E07C

MATRIX_LANE_TABLE               equ     $C3DD92
MATRIX_SECONDARY_TABLE          equ     $C3E5E8

prepare_c2e0_matrix_lane_tables:
                lea     MATRIX_LANE_TABLE.l,a1
                swap    d0
                asr.w   #4,d0
                bcc.b   $C2E08A
                addq.w  #1,d0
                add.w   d0,d0
                move.w  d0,d7
                blt.b   $C2E09E
                move.w  (a1,d0.w),d2
                move.w  d2,d4
                move.w  #$384,d3
                sub.w   d2,d3
                bra.b   $C2E0B2
                neg.w   d0
                move.w  (a1,d0.w),d1
                move.w  #$E10,d2
                sub.w   d1,d2
                move.w  d2,d4
                subi.w  #$A8C,d2
                move.w  d2,d3
                add.w   d3,d3
                lea     MATRIX_SECONDARY_TABLE.l,a0
                lea     MATRIX_LANE_TABLE.l,a1
                move.w  (a0,d3.w),d3
                cmpi.w  #$8F,d3
                bge.b   $C2E0DC
