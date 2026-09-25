; Byte-exact observed packed-nibble accumulator $C259C2-$C259FD.

                org     $C259C2

PACKED_NIBBLE_SOURCE             equ     $C45B22
PACKED_NIBBLE_ACCUMULATOR        equ     $C45B1E
PACKED_NIBBLE_TABLE              equ     $C25A3E
PACKED_NIBBLE_ADD_HELPER         equ     $C25A00

accumulate_c259c2_packed_nibbles:
                ; Preserve the captured predecrement MOVEM mask verbatim.
                dc.w    $48E7,$0706
                movea.l #PACKED_NIBBLE_SOURCE,a6
                lea     PACKED_NIBBLE_TABLE.l,a5
                moveq   #0,d7
                moveq   #3,d5
.next_byte:
                clr.w   d6
                move.b  (a6),d6
                lsr.b   #4,d6
                andi.w  #$F,d6
                bsr.w   PACKED_NIBBLE_ADD_HELPER
                move.b  (a6)+,d6
                andi.w  #$F,d6
                bsr.w   PACKED_NIBBLE_ADD_HELPER
                ; Preserve the captured DBF displacement verbatim.
                dc.w    $51CD,$FFE8
                move.l  d7,PACKED_NIBBLE_ACCUMULATOR.l
                ; Preserve the captured postincrement MOVEM mask verbatim.
                dc.w    $4CDF,$60E0
                rts
