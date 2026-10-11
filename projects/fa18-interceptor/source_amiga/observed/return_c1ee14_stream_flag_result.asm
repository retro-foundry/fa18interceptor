; Byte-exact observed C1EE14 stream-walk continuation $C1EE92-$C1EE9F.
; The shared byte flag selects either the later stream route or this immediate
; successful leaf return with D0 set to one.

                org     $C1EE92

STREAM_RESULT_FLAG             equ     $C45864

return_c1ee14_stream_flag_result:
                tst.b   STREAM_RESULT_FLAG.l
                beq.b   $C1EEDA
                moveq   #1,d0
                unlk    a6
                rts
