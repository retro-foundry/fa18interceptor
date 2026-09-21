; Byte-exact record-triple predicate setup $C20002-$C20055.
; The nonnegative branch starts at the separately unobserved $C20056.

                org     $C20002

OFFSET_VERTEX_TABLE             equ $C48390
TRIPLE_PREDICATE_WORKSPACE      equ $C4BF90

test_c20002_record_triple_predicate:
                lea     OFFSET_VERTEX_TABLE.l,a3
                lea     TRIPLE_PREDICATE_WORKSPACE.l,a0
                move.l  #4,(a0)+
                movem.w (a2)+,d1-d3
                move.l  0(a3,d1.w),(a0)+
                move.w  4(a3,d1.w),(a0)+
                move.l  0(a3,d2.w),(a0)+
                move.w  4(a3,d2.w),(a0)+
                move.l  0(a3,d3.w),(a0)+
                move.w  4(a3,d3.w),(a0)+
                movem.w -18(a0),d2-d7
                move.w  d4,d0
                and.w   d7,d0
                sub.w   d2,d5
                sub.w   d3,d6
                sub.w   d4,d7
                movem.w -6(a0),d2-d4
                and.w   d4,d0
                sub.w   d5,d2
                sub.w   d6,d3
                sub.w   d7,d4
                and.w   d4,d0
                bge.b   $C20056
