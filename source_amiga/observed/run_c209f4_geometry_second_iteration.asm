; Byte-exact second geometry iteration and return $C209F4-$C20A3F.
; Output and helper ownership remain structural.

                org     $C209F4

GEOMETRY_OUTPUT_TRIPLES         equ $C4C592
GEOMETRY_STAGE_HELPER            equ $C2EE4A

run_c209f4_geometry_second_iteration:
                movem.w d4-d6,-88(a6)
                asr.w   #1,d4
                asr.w   #1,d5
                asr.w   #1,d6
                movem.w -76(a6),d1-d3
                sub.w   d4,d1
                sub.w   d5,d2
                sub.w   d6,d3
                move.w  d1,d4
                move.w  d2,d5
                move.w  d3,d6
                sub.w   -70(a6),d4
                sub.w   -68(a6),d5
                sub.w   -66(a6),d6
                movem.w d1-d6,GEOMETRY_OUTPUT_TRIPLES.l
                jsr     GEOMETRY_STAGE_HELPER.l
                or.w    d0,-126(a6)
                subq.w  #1,-58(a6)
                bgt.b   $C209E2
                movem.l (sp)+,a1/a2/a5
                move.w  -126(a6),d0
                rts
