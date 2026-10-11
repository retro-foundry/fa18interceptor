; Byte-exact projection-component publication tail $C1C5E0-$C1C63D.
; The shifted D1 component becomes the renderer depth longword at C45A78.

                org     $C1C5E0

publish_projection_depth_component:
                move.l  $14(a2),d3
                andi.l  #$3fffff,d3
                add.l   d3,d0
                add.l   $18(a2),d1
                move.l  $1c(a2),d3
                andi.l  #$3fffff,d3
                add.l   d3,d2
                neg.l   d0
                neg.l   d1
                neg.l   d2
                movem.l d0-d2,$C45A62.l
                bra.b   .publish_components
.reload_alternate_components:
                bsr.w   $C1C2C8
                movem.l $C45C3E.l,d0-d2
                movem.l d0-d2,$C45A7C.l
                movem.l $C45A62.l,d0-d2
.publish_components:
                asr.l   #8,d0
                asr.l   #8,d1
                asr.l   #8,d2
                movem.w d0-d2,$C45A72.l
                move.l  d1,$C45A78.l
                rts
