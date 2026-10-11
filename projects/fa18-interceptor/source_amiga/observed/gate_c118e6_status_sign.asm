; Byte-exact observed status-sign gate $C118E6-$C118EF.

                org     $C118E6

C118FA                         equ     $C118FA
C457E0                         equ     $C457E0

gate_c118e6_status_sign:
                move.b  C457E0.l,d0
                tst.b   d0
                bpl.b   C118FA
