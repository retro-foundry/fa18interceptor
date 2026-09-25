; Byte-exact observed dual mask-row update paths $C32806-$C3287F.
; The zero-selector path at $C32880 is outside the captured execution.

                org     $C32806

MASK_ROWS_ZERO_SELECTOR          equ     $C32880
MASK_ROWS_MASKED                 equ     $C32858
MASK_ROWS_RETURN_TAIL            equ     $C3289A

update_c32806_mask_rows:
                ; Captured MOVEM mask is intentionally preserved verbatim.
                dc.w    $48E7,$C200
                swap    d2
                move.w  d3,d2
                swap    d2
                move.w  d7,d6
                movea.l d4,a0
                movea.l d1,a3
                lsr.w   #6,d6
                subq.w  #1,d6
                move.w  d2,d0
                rol.w   #4,d2
                andi.w  #$F,d2
                andi.w  #$F0,d0
                beq.b   MASK_ROWS_ZERO_SELECTOR
                andi.w  #$C0,d0
                bne.b   MASK_ROWS_MASKED
.inverted_rows:
                move.l  #$E0000000,d1
.inverted_loop:
                moveq   #0,d0
                move.b  (a0),d0
                ror.l   #8,d0
                lsr.l   d2,d1
                lsr.l   d2,d0
                move.l  (a3),d3
                not.l   d0
                and.l   d1,d0
                not.l   d1
                and.l   d3,d1
                or.l    d0,d1
                move.l  d1,(a3)
                addq.w  #1,a0
                ; Keep ADDA.W immediate encoding, not VASM's shortened ADD.W.
                dc.w    $D6FC,$0028
                ; Preserve the captured DBF displacement verbatim.
                dc.w    $51CE,$FFDA
                bra.b   MASK_ROWS_RETURN_TAIL
