; Byte-exact observed menu-mode setup and sign gate $C0FECE-$C0FEE9.

                org     $C0FECE

C1017A                         equ     $C1017A
C458A6                         equ     $C458A6
C45AD6                         equ     $C45AD6

initialize_c0fece_menu_mode_gate:
                link.w  a6,#-2
                move.b  C458A6.l,d0
                ext.w   d0
                move.w  C45AD6.l,d1
                move.w  d0,-2(a6)
                tst.w   d1
                bpl.w   C1017A
