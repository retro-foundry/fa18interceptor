; Byte-exact shared tuple-cache rejection return $C24982-$C24995.

                org     $C24982

PROJECTION_ERROR_SLOT           equ     $C4599E
REPORT_PROJECTION_ERROR         equ     $C06C02

return_tuple_cache_rejection:
                move.w  #4,PROJECTION_ERROR_SLOT.l
                jsr     REPORT_PROJECTION_ERROR.l
                movem.w (a7)+,d0-d2
                rts
