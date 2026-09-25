; Byte-exact observed second candidate-byte delta block $C27592-$C2759D.

                org     $C27592

prepare_c27592_candidate_second_delta:
                move.b  $9(a3),d1
                sub.b   (a1)+,d1
                move.b  d1,d2
                bge.b   $C2759E
                neg.b   d2
