; Byte-exact observed candidate-byte delta setup $C27576-$C27589.

                org     $C27576

prepare_c27576_candidate_delta:
                movea.l d1,a0
                ; ASL.W #6,D0; preserve the observed compact encoding.
                dc.w    $ed40
                adda.w  d0,a1
                addq.w  #2,a1
                moveq   #0,d0
                move.b  $7(a3),d0
                sub.b   (a1)+,d0
                move.b  d0,d2
                bge.b   $C2758C
