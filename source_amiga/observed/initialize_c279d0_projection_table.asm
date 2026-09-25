; Byte-exact observed table setup in $C279D0, $C27A80-$C27ADA.
; The BEQ target at $C27ADC is outside this bounded source slice.

                org     $C27A80

PROJECTION_TABLE_CONTINUE       equ     $C27AF4

initialize_c279d0_projection_table:
                move.w  (a3)+,-$8(a6)
                move.w  (a3)+,-$1a(a6)
                move.w  #$0800,d0
                move.w  d0,d1
                lsr.w   #1,d1
                move.w  d1,d2
                sub.w   -$6(a6),d1
                sub.w   -$2(a6),d2
                neg.w   d0
                and.w   d0,d1
                add.w   -$6(a6),d1
                move.w  d1,-$6(a6)
                and.w   d0,d2
                add.w   -$2(a6),d2
                move.w  d2,-$2(a6)
                move.l  a3,d0
                move.w  -$16(a6),d1
                beq.b   $C27ADC
                move.w  -$4(a6),d0
                asl.w   d1,d0
                move.w  d0,-$4(a6)
                move.l  #$C286DC,-$c(a6)
                move.l  #$C286F4,-$10(a6)
                move.l  #$C2870C,-$14(a6)
                bra.b   PROJECTION_TABLE_CONTINUE
