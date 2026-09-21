; Byte-exact leaf $C0DAD4-$C0DADB.
                org $C0DAD4
append_fixed_x_zero:
 move.w #$13f,(a1)+
 clr.w (a1)+
 rts
