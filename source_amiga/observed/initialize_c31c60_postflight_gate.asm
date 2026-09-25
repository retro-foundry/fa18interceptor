; Byte-exact observed postflight setup and gate $C31C60-$C31C85.

                org     $C31C60

C31C5E                         equ     $C31C5E
C457FA                         equ     $C457FA
C458DE                         equ     $C458DE
C45954                         equ     $C45954
C46184                         equ     $C46184

initialize_c31c60_postflight_gate:
                move.w  #$d,C45954.l
                lea     C46184.l,a1
                adda.w  C458DE.l,a1
                lea     C457FA.l,a2
                moveq   #0,d0
                move.b  $63(a1),d2
                andi.b  #$f0,d2
                beq.b   C31C5E
