; Byte-exact static-only offset renderer lane-mask targets $C2F8D0-$C2FA6F.

                org     $C2F8D0

; Each lane operation is applied at its base pointer and again at +$28.
offset_renderer_0:
 and.w d0,(a3)
 and.w d0,$28(a3)
 and.w d1,(a2)
 and.w d1,$28(a2)
 and.w d2,(a1)
 and.w d2,$28(a1)
 and.w d3,(a0)
 and.w d3,$28(a0)
 rts
offset_renderer_1:
 or.w d4,(a3)
 or.w d4,$28(a3)
 and.w d1,(a2)
 and.w d1,$28(a2)
 and.w d2,(a1)
 and.w d2,$28(a1)
 and.w d3,(a0)
 and.w d3,$28(a0)
 rts
offset_renderer_2:
 and.w d0,(a3)
 and.w d0,$28(a3)
 or.w d5,(a2)
 or.w d5,$28(a2)
 and.w d2,(a1)
 and.w d2,$28(a1)
 and.w d3,(a0)
 and.w d3,$28(a0)
 rts
offset_renderer_3:
 or.w d4,(a3)
 or.w d4,$28(a3)
 or.w d5,(a2)
 or.w d5,$28(a2)
 and.w d2,(a1)
 and.w d2,$28(a1)
 and.w d3,(a0)
 and.w d3,$28(a0)
 rts
offset_renderer_4:
 and.w d0,(a3)
 and.w d0,$28(a3)
 and.w d1,(a2)
 and.w d1,$28(a2)
 or.w d6,(a1)
 or.w d6,$28(a1)
 and.w d3,(a0)
 and.w d3,$28(a0)
 rts
offset_renderer_5:
 or.w d4,(a3)
 or.w d4,$28(a3)
 and.w d1,(a2)
 and.w d1,$28(a2)
 or.w d6,(a1)
 or.w d6,$28(a1)
 and.w d3,(a0)
 and.w d3,$28(a0)
 rts
offset_renderer_6:
 and.w d0,(a3)
 and.w d0,$28(a3)
 or.w d5,(a2)
 or.w d5,$28(a2)
 or.w d6,(a1)
 or.w d6,$28(a1)
 and.w d3,(a0)
 and.w d3,$28(a0)
 rts
offset_renderer_7:
 or.w d4,(a3)
 or.w d4,$28(a3)
 or.w d5,(a2)
 or.w d5,$28(a2)
 or.w d6,(a1)
 or.w d6,$28(a1)
 and.w d3,(a0)
 and.w d3,$28(a0)
 rts
offset_renderer_8:
 and.w d0,(a3)
 and.w d0,$28(a3)
 and.w d1,(a2)
 and.w d1,$28(a2)
 and.w d2,(a1)
 and.w d2,$28(a1)
 or.w d7,(a0)
 or.w d7,$28(a0)
 rts
offset_renderer_9:
 or.w d4,(a3)
 or.w d4,$28(a3)
 and.w d1,(a2)
 and.w d1,$28(a2)
 and.w d2,(a1)
 and.w d2,$28(a1)
 or.w d7,(a0)
 or.w d7,$28(a0)
 rts
offset_renderer_10:
 and.w d0,(a3)
 and.w d0,$28(a3)
 or.w d5,(a2)
 or.w d5,$28(a2)
 and.w d2,(a1)
 and.w d2,$28(a1)
 or.w d7,(a0)
 or.w d7,$28(a0)
 rts
offset_renderer_11:
 or.w d4,(a3)
 or.w d4,$28(a3)
 or.w d5,(a2)
 or.w d5,$28(a2)
 and.w d2,(a1)
 and.w d2,$28(a1)
 or.w d7,(a0)
 or.w d7,$28(a0)
 rts
offset_renderer_12:
 and.w d0,(a3)
 and.w d0,$28(a3)
 and.w d1,(a2)
 and.w d1,$28(a2)
 or.w d6,(a1)
 or.w d6,$28(a1)
 or.w d7,(a0)
 or.w d7,$28(a0)
 rts
offset_renderer_13:
 or.w d4,(a3)
 or.w d4,$28(a3)
 and.w d1,(a2)
 and.w d1,$28(a2)
 or.w d6,(a1)
 or.w d6,$28(a1)
 or.w d7,(a0)
 or.w d7,$28(a0)
 rts
offset_renderer_14:
 and.w d0,(a3)
 and.w d0,$28(a3)
 or.w d5,(a2)
 or.w d5,$28(a2)
 or.w d6,(a1)
 or.w d6,$28(a1)
 or.w d7,(a0)
 or.w d7,$28(a0)
 rts
offset_renderer_15:
 or.w d4,(a3)
 or.w d4,$28(a3)
 or.w d5,(a2)
 or.w d5,$28(a2)
 or.w d6,(a1)
 or.w d6,$28(a1)
 or.w d7,(a0)
 or.w d7,$28(a0)
 rts
