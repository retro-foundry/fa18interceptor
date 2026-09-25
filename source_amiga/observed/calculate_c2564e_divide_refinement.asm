; Byte-exact observed divide/refinement path $C2564E-$C25691.
; The high-value branch destination is outside this observed path.

                org     $C2564E

C25692                         equ     $C25692
C45B64                         equ     $C45B64
C45B68                         equ     $C45B68

calculate_c2564e_divide_refinement:
                movem.l d0-d3,-(sp)
                move.l  C45B64.l,d0
                cmpi.l  #$63f000,d0
                bgt.w   C25692
                move.l  d0,d2
                divu.w  #$c8,d2
                addq.w  #2,d2
.refine:
                move.l  d0,d1
                divu.w  d2,d1
                move.w  d1,d3
                sub.w   d2,d3
                beq.b   .store
                cmpi.w  #1,d3
                beq.b   .store
                cmpi.w  #-1,d3
                beq.b   .store
                add.w   d1,d2
                lsr.w   #1,d2
                bra.b   .refine
.store:
                move.w  d1,C45B68.l
                movem.l (sp)+,d0-d3
                rts
