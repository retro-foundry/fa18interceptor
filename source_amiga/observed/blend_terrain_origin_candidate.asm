; Byte-exact candidate blend $C2940A-$C29487.
; The active-record triple and live selector origin are repeatedly averaged to
; produce C45C56.  Field meanings and physical-coordinate semantics remain
; unassigned.

                org     $C2940A

ORIGIN_RECORD_BASE        equ     $C46184
ORIGIN_RECORD_OFFSET      equ     $C458DE
TERRAIN_SELECTOR_ORIGIN   equ     $C45C3E
ORIGIN_CANDIDATE_TRIPLE   equ     $C45C56

blend_terrain_origin_candidate:
                movem.l d3-d6,-(a7)
                lea.l   ORIGIN_RECORD_BASE.l,a2
                adda.w  ORIGIN_RECORD_OFFSET.l,a2
                movem.l TERRAIN_SELECTOR_ORIGIN.l,d0-d2
                add.l   $14(a2),d0
                add.l   $18(a2),d1
                add.l   $1c(a2),d2
                asr.l   #1,d0
                asr.l   #1,d1
                asr.l   #1,d2
                movem.l d0-d2,-(a7)
                add.l   TERRAIN_SELECTOR_ORIGIN.l,d0
                add.l   $C45C46.l,d2
                asr.l   #1,d0
                asr.l   #1,d1
                asr.l   #1,d2
                movem.l (a7)+,d3-d5
                add.l   d3,d0
                add.l   d4,d1
                add.l   d5,d2
                asr.l   #1,d0
                asr.l   #1,d1
                asr.l   #1,d2
                movem.l d0-d2,-(a7)
                add.l   d3,d0
                add.l   d4,d1
                add.l   d5,d2
                asr.l   #1,d0
                asr.l   #1,d1
                asr.l   #1,d2
                movem.l (a7)+,d3-d5
                add.l   d3,d0
                add.l   d4,d1
                add.l   d5,d2
                asr.l   #1,d0
                asr.l   #1,d1
                asr.l   #1,d2
                movem.l d0-d2,ORIGIN_CANDIDATE_TRIPLE.l
                movem.l (a7)+,d3-d6
                bra.b   $C29506
