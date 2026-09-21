; Byte-exact leaf $C0DADC-$C0DAE5.
                org $C0DADC
append_fixed_screen_pair:
 move.w #$13f,(a1)+
 move.w #$b3,(a1)+
 rts
