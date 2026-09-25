; Byte-exact observed candidate-scan result setup $C27968-$C27973.

                org     $C27968

prepare_c27968_candidate_result:
                move.l  $10(a3),d1
                move.l  d1,d2
                move.b  $7b(a3),d0
                blt.b   $C279B8
