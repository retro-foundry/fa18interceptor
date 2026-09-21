; Byte-exact $C2EC36-$C2EC67 shared return tail for $C2EA5A/$C2EAD0.
; Returns D0=0 only when both adjusted signed components fit the D2 bound.
                org $C2EC36
ADJUSTED_PAIR_SNAPSHOT equ $C45AC6
classify_adjusted_display_pair_bounds:
 movem.w d0-d2,ADJUSTED_PAIR_SNAPSHOT.l
 move.w d2,d5
 blt.b .reject
 move.w d2,d6
 cmp.w d5,d0
 bgt.b .reject
 neg.w d0
 cmp.w d5,d0
 bgt.b .reject
 cmp.w d6,d1
 bgt.b .reject
 neg.w d1
 cmp.w d6,d1
 ble.b .accept
.reject:
 moveq #1,d0
 movem.l (sp)+,d0-d6
 rts
.accept:
 moveq #0,d0
 movem.l (sp)+,d0-d6
 rts
