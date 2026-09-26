; Byte-exact context-refresh stage-C prefix $C09A78-$C09A83.
; It supplies the first static descriptor table to the shared selector helper;
; a zero helper result chooses the clear-state continuation.

                org     $C09A78

CONTEXT_REFRESH_TABLE_CLEAR    equ     $C09B48
CONTEXT_REFRESH_SELECTOR        equ     $C09AB8

update_context_refresh_stage_c_clear:
                lea     CONTEXT_REFRESH_TABLE_CLEAR.l,a0
                bsr.w   CONTEXT_REFRESH_SELECTOR
                beq.b   $C09A8E
