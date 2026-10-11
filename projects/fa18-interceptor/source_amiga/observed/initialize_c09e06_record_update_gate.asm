; Byte-exact observed update-helper setup and gates $C09E06-$C09E3B.

                org     $C09E06

C09E94                         equ     $C09E94
C230B0                         equ     $C230B0
C45790                         equ     $C45790
C457BA                         equ     $C457BA
C457BB                         equ     $C457BB
C457BC                         equ     $C457BC
C458A6                         equ     $C458A6
C458CD                         equ     $C458CD

initialize_c09e06_record_update_gate:
                clr.b   C457BA.l
                clr.b   C457BB.l
                clr.b   C457BC.l
                jsr     C230B0.l
                btst    #6,C458CD.l
                beq.b   C09E94
                tst.b   C45790.l
                bne.b   C09E94
                move.b  C458A6.l,d0
                cmpi.b  #2,d0
                ble.b   C09E94
