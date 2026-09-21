; Byte-exact observed-entry function $C21060-$C210E5 (Hunk 13 +$378).
; A2 is a signed-word sentinel list followed by three word offsets per entry.
; The semantic identity of the records is not yet assigned.

                org     $C21060

DISPLAY_WORK_FLAGS             equ -$7E
DISPLAY_TUPLE_COUNT            equ $C45954
LINE_EMITTER_INPUT             equ $C456E6
TUPLE_WORKSPACE_HEADER         equ $C4BF90
TUPLE_WORKSPACE                equ $C4BF94
OFFSET_TUPLE_TABLE             equ $C48390
PROCESS_TUPLE_WORKSPACE        equ $C246A0
INITIAL_TUPLE_COUNT            equ 4
INITIAL_LINE_X                 equ 8
INITIAL_LINE_Y                 equ 8

dispatch_offset_tuple_records:
                movem.l a1/a5,-(a7)
                move.w  #$000C,DISPLAY_TUPLE_COUNT.l
                move.w  #INITIAL_LINE_X,d0
                move.w  #INITIAL_LINE_Y,d1
                clr.w   d2
                clr.w   d3
                movem.w d0-d3,LINE_EMITTER_INPUT.l
                clr.w   DISPLAY_WORK_FLAGS(a6)
                move.l  #INITIAL_TUPLE_COUNT,TUPLE_WORKSPACE_HEADER.l

.next_record:
                move.w  (a2)+,d1
                blt.s   .done
                movem.w (a2)+,d2-d4
                lea     TUPLE_WORKSPACE.l,a0
                lea     OFFSET_TUPLE_TABLE.l,a3

                lea     (a3,d1.w),a4
                move.l  (a4)+,(a0)+
                move.w  (a4),d6
                move.w  d6,(a0)+
                lea     (a3,d2.w),a4
                move.l  (a4)+,(a0)+
                move.w  (a4),(a0)+
                and.w   (a4),d6
                lea     (a3,d3.w),a4
                move.l  (a4)+,(a0)+
                move.w  (a4),(a0)+
                and.w   (a4),d6
                lea     (a3,d4.w),a4
                move.l  (a4)+,(a0)+
                move.w  (a4),(a0)+
                and.w   (a4),d6
                blt.s   .next_record

                move.l  a2,-(a7)
                jsr     PROCESS_TUPLE_WORKSPACE.l
                or.w    d0,DISPLAY_WORK_FLAGS(a6)
                movea.l (a7)+,a2
                bra.s   .next_record

.done:
                movem.l (a7)+,a1/a5
                move.w  DISPLAY_WORK_FLAGS(a6),d0
                rts