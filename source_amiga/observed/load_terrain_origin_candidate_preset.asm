; Byte-exact candidate-preset loader $C29488-$C294AB.
; Threshold cases select one of two static triples and publish it as C45C56.

                org     $C29488

ORIGIN_CANDIDATE_TRIPLE   equ     $C45C56

load_terrain_origin_candidate_preset:
                bsr.w   .load_c46198_preset
                bra.w   $C29506
.load_c46998_preset:
                movem.l $C46998.l,d0-d2
                bra.b   .publish_candidate_preset
.load_c46198_preset:
                movem.l $C46198.l,d0-d2
.publish_candidate_preset:
                movem.l d0-d2,ORIGIN_CANDIDATE_TRIPLE.l
                rts
