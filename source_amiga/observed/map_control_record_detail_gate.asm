; Byte-exact observed map/control record gate $C2AD00-$C2AD7F.
; The record metric at -$28(A6) has threshold bands only when the alternate
; mode word at -$3E(A6) is non-zero.  Names describe dataflow, not object type.

                org     $C2AD00

map_control_record_dispatch:
                tst.w   -$44(a6)
                bne.w   $C2AFF8
                move.b  (a0)+,d2
                ext.w   d2
                cmpi.b  #$ff,d2
                beq.w   $C2AFF8
                bge.b   .record_mode_ready
                addq.w  #1,-$44(a6)
                andi.w  #$7f,d2
.record_mode_ready:
                clr.w   -$22(a6)
                clr.w   -$20(a6)
                clr.b   -$24(a6)
                tst.w   -$3e(a6)
                beq.b   $C2AD80
                tst.b   $C457AD.l
                bne.b   $C2AD80
                cmpi.w  #4,d2
                bne.b   .non_four_mode
                move.b  $C4589C.l,-$24(a6)
                cmpi.l  #$400,-$28(a6)
                bgt.b   .four_middle_band
                move.w  #2,-$20(a6)
                bra.b   $C2AD80
.four_middle_band:
                cmpi.l  #$c80,-$28(a6)
                bgt.b   $C2AD80
                move.w  #1,-$20(a6)
                bra.b   $C2AD80
.non_four_mode:
                cmpi.l  #$c80,-$28(a6)
                bgt.b   $C2AD80
                move.w  #1,-$20(a6)
                move.w  #1,-$22(a6)
