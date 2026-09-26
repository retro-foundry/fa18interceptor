; Byte-exact observed record signed-component route $C2C310-$C2C33F.
; Record status bits and signed word +$4C gate this route.  It scales word
; +$6C into a longword, then compares it with record components +$42/+18.

                org     $C2C310

gate_record_signed_component_route:
                btst    #1,$20(a1)
                bne.w   $C2CC26
                btst    #0,$2(a1)
                bne.b   $C2C366
                tst.w   $4C(a1)
                blt.w   $C2C47E
                move.w  $6C(a1),d4
                ext.l   d4
                asl.l   #6,d4
                tst.l   $42(a1)
                bge.b   $C2C352
                cmp.l   $18(a1),d4
                blt.w   $C2CC26
