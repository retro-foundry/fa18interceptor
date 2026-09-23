; Byte-exact static adjustment/publish tail $C29548-$C295D0.
; This tail receives a magnitude in D3 and a three-component candidate in
; D5-D7.  It reduces over-large candidates, obtains a scale from $C2574A,
; smooths against C45C4A, adds the live selector origin, then publishes both
; the origin and its masked/negated companion.  The caller-selected threshold
; policy and any flight-distance interpretation remain unproven.

                org     $C29548

ORIGIN_SMOOTHED_DELTA     equ     $C45C4A
TERRAIN_SELECTOR_ORIGIN   equ     $C45C3E
ORIGIN_NEGATED_COMPANION  equ     $C45C32

adjust_and_publish_terrain_selector_origin:
.reduce_large_candidate:
                cmpi.l  #$4800,d3
                blt.b   .scale_candidate
                asr.l   #2,d5
                asr.l   #2,d6
                asr.l   #2,d7
                asr.l   #2,d3
                bra.b   .reduce_large_candidate
.scale_candidate:
                move.w  d4,-(a7)
                move.w  #$200,d0
                jsr     $C2574A.l
                move.w  (a7)+,d4
                ext.l   d5
                ext.l   d6
                ext.l   d7
                asl.l   d4,d5
                asl.l   d4,d6
                asl.l   d4,d7
                movem.l ORIGIN_SMOOTHED_DELTA.l,d0-d2
                move.l  d0,d3
                or.l    d1,d3
                or.l    d2,d3
                beq.b   .publish_smoothed_delta
                sub.l   d0,d5
                sub.l   d1,d6
                sub.l   d2,d7
                moveq   #1,d4
                asr.l   d4,d5
                asr.l   d4,d6
                asr.l   d4,d7
                add.l   d0,d5
                add.l   d1,d6
                add.l   d2,d7
.publish_smoothed_delta:
                movem.l d5-d7,ORIGIN_SMOOTHED_DELTA.l
                movem.l TERRAIN_SELECTOR_ORIGIN.l,d0-d2
                add.l   d0,d5
                add.l   d1,d6
                add.l   d2,d7
                movem.l d5-d7,TERRAIN_SELECTOR_ORIGIN.l
.publish_negated_companion:
                andi.l  #$3fffff,d5
                andi.l  #$3fffff,d7
                neg.l   d5
                neg.l   d6
                neg.l   d7
                movem.l d5-d7,ORIGIN_NEGATED_COMPANION.l
                rts
