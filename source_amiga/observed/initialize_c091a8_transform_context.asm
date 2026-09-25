; Byte-exact observed transform-context setup $C091A8-$C091CD.

                org     $C091A8

C091F6                         equ     $C091F6
C2DAF2                         equ     $C2DAF2
C458DE                         equ     $C458DE
C45C0E                         equ     $C45C0E
C46184                         equ     $C46184

initialize_c091a8_transform_context:
                movem.l a1-a2,-(sp)
                movem.w d3-d5,-(sp)
                jsr     C2DAF2.l
                movem.w (sp)+,d3-d5
                lea     C46184.l,a1
                adda.w  C458DE.l,a1
                lea     C45C0E.l,a2
                bra.b   C091F6
