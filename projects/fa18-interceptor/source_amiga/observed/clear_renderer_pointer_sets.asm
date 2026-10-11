; Byte-exact static-only renderer pointer-set clear $C2F582-$C2F5BF.
; C2F582 clears the base set, then C2F58C selects and clears the offset set.

                org     $C2F582

RENDERER_POINTER_SET_BASE       equ $C4566E
RENDERER_POINTER_SET_OFFSET     equ $10
BSR_W_CLEAR_POINTER_SET         equ $6100
CLEAR_POINTER_SET_DISPLACEMENT  equ $000C
ADDA_W_IMMEDIATE_A1_OPCODE       equ $D2FC

clear_renderer_pointer_sets:
                lea.l   RENDERER_POINTER_SET_BASE.l,a1
                ; VASM relaxes this same-file BSR; retain its original BSR.W.
                dc.w    BSR_W_CLEAR_POINTER_SET,CLEAR_POINTER_SET_DISPLACEMENT
                lea.l   RENDERER_POINTER_SET_BASE.l,a1
                ; VASM rewrites this to LEA; retain the original ADDA.W.
                dc.w    ADDA_W_IMMEDIATE_A1_OPCODE,RENDERER_POINTER_SET_OFFSET

clear_renderer_pointer_set:
                movem.l (a1),a0-a3
                moveq   #0,d0
                move.l  d0,d1
                move.l  d0,d2
                move.l  d0,d3
                move.l  d0,d4
                move.l  d0,d5
                move.l  d0,d6
                move.l  d0,d7
                movea.l d0,a4
                movea.l d0,a5
                movem.l d0-d7/a4-a5,(a0)
                movem.l d0-d7/a4-a5,(a1)
                movem.l d0-d7/a4-a5,(a2)
                movem.l d0-d7/a4-a5,(a3)
                rts
