; Byte-exact terminal paired-record route selection $C231F0-$C23227.
; Zero is the ordinary route; the nonzero route consumes C457BC.

                org     $C231F0

C457BC                         equ     $C457BC

return_c231a2_route_result:
                move.b  $64(a2),d0
                andi.b  #$60,d0
                cmpi.b  #$60,d0
                bne.b   return_zero_c231a2
                btst    #0,1(a2)
                beq.b   return_zero_c231a2
                move.b  $63(a2),d0
                andi.b  #$f0,d0
                cmpi.b  #$20,d0
                beq.b   return_one_c231a2
                cmpi.b  #$30,d0
                beq.b   return_one_c231a2

return_zero_c231a2:
                moveq   #0,d0
                rts

return_one_c231a2:
                clr.b   C457BC.l
                moveq   #1,d0
                rts
