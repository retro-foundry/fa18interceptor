; Byte-exact observed candidate-scan zero-return route $C279B8-$C279C1.

                org     $C279B8

return_c279b8_candidate_result:
                tst.l   d1
                blt.b   $C279C2
                moveq   #0,d0
                unlk    a6
                rts
