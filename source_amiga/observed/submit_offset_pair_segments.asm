; Byte-exact observed-entry routine $C212B0-$C2131B.
; A2 supplies endpoint-offset pairs; a negative second word marks the final
; pair while its masked low 15 bits remain a usable endpoint offset.
; Each endpoint selects a three-word record in C48390; the pair is projected.

DISPLAY_EDGE_SELECTOR          equ     $C45954
LINE_EMITTER_MASK              equ     $C456E6
EDGE_RESULT_FLAGS              equ     -$7E
EDGE_LIST_TERMINATED           equ     -$6E
OFFSET_VERTEX_TABLE            equ     $C48390
PROJECT_PAIR_WORKSPACE         equ     $C4C592
PROJECT_AND_SUBMIT_PAIR        equ     $C2EE4A

                org     $C212B0

submit_offset_pair_segments:
                movem.l a1/a5,-(a7)
                move.l  #-$1,LINE_EMITTER_MASK.l
                move.w  (a2)+,DISPLAY_EDGE_SELECTOR.l
                clr.w   EDGE_RESULT_FLAGS(a6)
                clr.w   EDGE_LIST_TERMINATED(a6)
submit_offset_pair_next:
                tst.w   EDGE_LIST_TERMINATED(a6)
                bne.s   submit_offset_pair_done
                move.w  (a2)+,d1
                move.w  (a2)+,d2
                bge.s   submit_offset_pair_resolve
                addq.w  #1,EDGE_LIST_TERMINATED(a6)
                andi.w  #$7FFF,d2
submit_offset_pair_resolve:
                lea.l   OFFSET_VERTEX_TABLE.l,a3
                lea.l   PROJECT_PAIR_WORKSPACE.l,a0
                lea.l   (a3,d1.w),a4
                move.l  (a4)+,(a0)+
                move.w  (a4),d6
                move.w  d6,(a0)+
                lea.l   (a3,d2.w),a4
                move.l  (a4)+,(a0)+
                dc.w    $CC54                   ; and.w (a4),d6; retain original opcode
                blt.s   submit_offset_pair_skip
                move.w  (a4),(a0)
                move.l  a2,-(a7)
                jsr     PROJECT_AND_SUBMIT_PAIR.l
                or.w    d0,EDGE_RESULT_FLAGS(a6)
                movea.l (a7)+,a2
submit_offset_pair_skip:
                bra.s   submit_offset_pair_next
submit_offset_pair_done:
                movem.l (a7)+,a1/a5
                move.w  EDGE_RESULT_FLAGS(a6),d0
                rts
