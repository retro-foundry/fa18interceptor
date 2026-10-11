; Byte-exact observed candidate-scan zero-return route $C279B8-$C279C1.

                org     $C279B8

return_c279b8_candidate_result:
                tst.l   d1
                blt.b   $C279C2
                moveq   #0,d0
                unlk    a6
                rts

; Byte-exact negative accumulated-candidate return $C279C2-$C279C7.
; Run062 reaches this return with D1 negative and receives D0=$10.

                org     $C279C2

return_c279c2_negative_candidate_result:
                moveq   #$10,d0
                unlk    a6
                rts
