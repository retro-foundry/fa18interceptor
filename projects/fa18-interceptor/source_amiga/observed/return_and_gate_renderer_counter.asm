; Byte-exact renderer return and counter gate $C30C6E-$C30C77.

                org     $C30C6E

return_renderer_submission:
                rts

gate_renderer_counter_positive:
                tst.b   $C45842.l
