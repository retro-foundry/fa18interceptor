; Byte-exact observed entry gates and dispatch preparation $C2C392-$C2C3FD.
; Branch destinations in this slice are not reconstructed here.

                org     $C2C392

C2C38A                         equ     $C2C38A
C2C38E                         equ     $C2C38E
C2C400                         equ     $C2C400
C2C40E                         equ     $C2C40E
C2C48A                         equ     $C2C48A
C457AE                         equ     $C457AE
C457AF                         equ     $C457AF

prepare_c2c392_dispatch_gate:
                link.w  a6,#-$2c
                move.b  $62(a1),d0
                andi.b  #$f0,d0
                cmpi.b  #$30,d0
                beq.b   C2C38A
                move.w  #$1400,$7e(a1)
                ; Preserve the observed zero-displacement address encoding.
                dc.w    $3029,$0000
                andi.w  #$80,d0
                beq.b   C2C38A
                move.b  C457AF.l,d0
                or.b    C457AE.l,d0
                bne.b   C2C38E
                move.b  5(a1),d0
                beq.w   C2C48A
                cmpi.b  #8,5(a1)
                beq.b   C2C40E
                cmpi.b  #1,5(a1)
                beq.b   C2C40E
                btst    #1,$20(a1)
                bne.b   C2C40E
                btst    #0,2(a1)
                bne.b   C2C40E
                move.w  $6c(a1),d4
                ext.l   d4
                asl.l   #6,d4
                tst.l   $42(a1)
                bge.b   C2C400
                cmp.l   $18(a1),d4
                blt.b   C2C40E
