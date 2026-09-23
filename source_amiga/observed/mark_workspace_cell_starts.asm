; Byte-exact 16-cell strided marker helper $C1D722-$C1D763.
; Caller supplies D0 marker word, D1 stride, and A1 first cell.  The observed
; terrain-band reset uses D0=$FFFF and D1=$60; this helper itself is generic.

                org     $C1D722

mark_sixteen_workspace_cell_starts:
                move.w  d0,(a1)
                adda.w  d1,a1
                move.w  d0,(a1)
                adda.w  d1,a1
                move.w  d0,(a1)
                adda.w  d1,a1
                move.w  d0,(a1)
                adda.w  d1,a1
                move.w  d0,(a1)
                adda.w  d1,a1
                move.w  d0,(a1)
                adda.w  d1,a1
                move.w  d0,(a1)
                adda.w  d1,a1
                move.w  d0,(a1)
                adda.w  d1,a1
                move.w  d0,(a1)
                adda.w  d1,a1
                move.w  d0,(a1)
                adda.w  d1,a1
                move.w  d0,(a1)
                adda.w  d1,a1
                move.w  d0,(a1)
                adda.w  d1,a1
                move.w  d0,(a1)
                adda.w  d1,a1
                move.w  d0,(a1)
                adda.w  d1,a1
                move.w  d0,(a1)
                adda.w  d1,a1
                move.w  d0,(a1)
                adda.w  d1,a1
                rts
