; Byte-exact leaf $C0DAE6-$C0DAED.
                org $C0DAE6
append_zero_fixed_y:
 clr.w (a1)+
 move.w #$b3,(a1)+
 rts
