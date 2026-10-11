; Byte-exact first geometry iteration $C20996-$C209D9.
; Output and helper ownership remain structural.

                org     $C20996

GEOMETRY_OUTPUT_TRIPLES         equ $C4C592
GEOMETRY_STAGE_HELPER            equ $C2EE4A

run_c20996_geometry_first_iteration:
                movem.w d4-d6,-88(a6)
                asr.w   #1,d4
                asr.w   #1,d5
                asr.w   #1,d6
                movem.w (a3),d1-d3
                add.w   d4,d1
                add.w   d5,d2
                add.w   d6,d3
                move.w  d1,d4
                move.w  d2,d5
                move.w  d3,d6
                add.w   -70(a6),d4
                add.w   -68(a6),d5
                add.w   -66(a6),d6
                movem.w d1-d6,GEOMETRY_OUTPUT_TRIPLES.l
                move.l  a3,-(sp)
                jsr     GEOMETRY_STAGE_HELPER.l
                or.w    d0,-126(a6)
                movea.l (sp)+,a3
                subq.w  #1,-56(a6)
                bgt.b   $C20984
