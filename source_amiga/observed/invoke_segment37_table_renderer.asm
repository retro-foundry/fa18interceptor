; Byte-exact static-only segment-37 table renderer wrapper $C309B6-$C309E1.

                org     $C309B6

SEGMENT37_REMAINING_POINTERS     equ $C309A6
SEGMENT37_FIRST_POINTER          equ $C309A2
SEGMENT37_TABLE_RENDERER         equ $C30EAA

invoke_segment37_table_renderer:
                lea     SEGMENT37_REMAINING_POINTERS(pc),a1
                lea     SEGMENT37_FIRST_POINTER(pc),a3
                move.w  #$FCE,d2
                movea.l (a3),a3
                move.l  (a3),d0
                move.l  #$14CA,d1
                movea.w #$12,a4
                move.w  #$312,d6
                movea.w #5,a5
                move.w  #1,d7
                bsr.w   SEGMENT37_TABLE_RENDERER
                rts
