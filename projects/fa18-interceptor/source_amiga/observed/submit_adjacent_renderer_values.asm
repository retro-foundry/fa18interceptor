; Byte-exact $C2F60A-$C2F621 wrapper (Hunk 36 +$1D7A).
; Calls the shared renderer entry twice for low-nibble-zero input.

                org     $C2F60A

RENDER_SHARED_ENTRY            equ $C2F5F4
NONZERO_NIBBLE_ENTRY           equ $C2F626
LOW_NIBBLE_MASK                equ $000F

submit_adjacent_renderer_values:
                move.w  d0,d2
                andi.w  #LOW_NIBBLE_MASK,d2
                bne.s   NONZERO_NIBBLE_ENTRY
                movem.w d0-d1,-(a7)
                bsr.s   RENDER_SHARED_ENTRY
                movem.w (a7)+,d0-d1
                subq.w  #1,d0
                bsr.s   RENDER_SHARED_ENTRY
                rts
