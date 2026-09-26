; Byte-exact context-refresh stage-C clear tail $C09A8E-$C09A97.

                org     $C09A8E

CONTEXT_REFRESH_STAGE_C_FLAG    equ     $C4589B

clear_context_refresh_stage_c:
                move.b  #0,CONTEXT_REFRESH_STAGE_C_FLAG.l
                rts
