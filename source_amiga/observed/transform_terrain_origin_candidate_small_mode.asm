; Byte-exact candidate transform path $C294AC-$C29505.
; Two threshold cases choose small signed constants, call C091CE, and publish
; its output as C45C56.  The input/control meanings are not assigned.

                org     $C294AC

ORIGIN_CANDIDATE_TRIPLE   equ     $C45C56
ORIGIN_VARIANT_SELECTOR   equ     $C45848

transform_terrain_origin_candidate_small_mode:
                movem.l d3-d7,-(a7)
                moveq   #4,d4
                cmpi.b  #1,ORIGIN_VARIANT_SELECTOR.l
                beq.b   .use_positive_constants
                cmpi.b  #2,ORIGIN_VARIANT_SELECTOR.l
                beq.b   .use_positive_constants
                moveq   #$fa,d3
                moveq   #$f7,d5
                bra.b   .transform_candidate
.use_positive_constants:
                moveq   #6,d3
                moveq   #3,d5
                bra.b   .transform_candidate
.alternate_small_mode:
                movem.l d3-d7,-(a7)
                moveq   #1,d4
                moveq   #6,d5
                cmpi.b  #1,ORIGIN_VARIANT_SELECTOR.l
                beq.b   .use_alternate_positive_d3
                cmpi.b  #2,ORIGIN_VARIANT_SELECTOR.l
                beq.b   .use_alternate_positive_d3
                moveq   #$fa,d3
                bra.b   .transform_candidate
.use_alternate_positive_d3:
                moveq   #6,d3
.transform_candidate:
                jsr     $C091CE.l
                movem.l d0-d2,ORIGIN_CANDIDATE_TRIPLE.l
                movem.l (a7)+,d3-d7
