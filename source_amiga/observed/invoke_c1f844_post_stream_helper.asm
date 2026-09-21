; Byte-exact post-stream helper invocation $C1F8D8-$C1F8E5.

                org     $C1F8D8

POST_STREAM_HELPER              equ $C0D04C
RECORD_STATUS_LOCAL             equ -124

invoke_c1f844_post_stream_helper:
                jsr     POST_STREAM_HELPER.l
                move.w  RECORD_STATUS_LOCAL(a6),d0
                unlk    a6
                rts
