; Byte-exact observed C13D84 continuation $C13EA0-$C13EE9.
; It conditionally adds the local bounded nibble term to a record byte and
; branches on two record-word flag bits.  Field roles remain structural.

                org     $C13EA0

update_c13d84_bounded_byte_term:
                movea.l -$8(a6),a0
                move.b  (a0),d0
                andi.b  #3,d0
                subq.b  #1,d0
                bne.w   $C13F48
                movea.l -$30(a6),a0
                move.w  (a0),d0
                andi.w  #$FBFF,d0
                move.w  d0,(a0)
                movea.l -$4(a6),a0
                move.b  (a0),d1
                ext.w   d1
                ext.l   d1
                move.w  -$16(a6),d2
                ext.l   d2
                add.l   d2,d1
                move.b  d1,(a0)
                cmpi.b  #$78,d1
                blt.w   $C14008
                movea.l -$30(a6),a0
                move.w  (a0),d0
                btst    #$D,d0
                beq.b   $C13F18
                btst    #3,d0
                bne.b   $C13F3C
