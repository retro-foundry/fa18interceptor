; Byte-exact observed mask-update packet $C330FE-$C33168.
; run029's normal full-frame profiler executes every instruction in both loop
; forms. Record ownership and the semantic meaning of its mode inputs remain
; unassigned.
                org     $C330FE

STRIDED_LONG_BYTES      equ     $28

apply_byte_masks_to_strided_longs:
                move.l  d0,-(sp)
                move.l  d6,-(sp)
                swap    d2
                move.w  d3,d2
                swap    d2
                move.w  d7,d6
                movea.l d4,a0
                movea.l d1,a3
                lsr.w   #6,d6
                subq.w  #1,d6
.next_mask:
                move.w  d2,d0
                rol.w   #4,d2
                andi.w  #$000f,d2
                andi.w  #$00f0,d0
                bne.b   .set_masked_bits
.clear_masked_bits:
                moveq   #0,d0
                move.b  (a0),d0
                asl.w   #8,d0
                swap    d0
                move.l  (a3),d3
                lsr.l   d2,d0
                not.l   d0
                and.l   d0,d3
                move.l  d3,(a3)
                addq.w  #1,a0
                dc.w    $d6fc,STRIDED_LONG_BYTES ; adda.w #$28,a3; retain original non-relaxed opcode
                dbra    d6,.clear_masked_bits
                bra.b   .restore
.set_masked_bits:
                moveq   #0,d0
                move.b  (a0),d0
                asl.w   #8,d0
                swap    d0
                move.l  (a3),d3
                lsr.l   d2,d0
                not.l   d0
                and.l   d0,d3
                not.l   d0
                or.l    d0,d3
                move.l  d3,(a3)
                addq.w  #1,a0
                dc.w    $d6fc,STRIDED_LONG_BYTES ; adda.w #$28,a3; retain original non-relaxed opcode
                dbra    d6,.set_masked_bits
.restore:
                swap    d2
                move.w  d2,d3
                swap    d6
                move.l  (sp)+,d6
                move.l  (sp)+,d0
                rts
