; Byte-exact variable record-triple preparation $C2005C-$C200A7.
; Table and workspace ownership remain structural.

                org     $C2005C

TRIPLE_PREDICATE_WORKSPACE      equ $C4BF90
OFFSET_VERTEX_TABLE             equ $C48390

prepare_c2005c_variable_record_triples:
                lea     TRIPLE_PREDICATE_WORKSPACE.l,a0
                clr.w   (a0)+
                addq.w  #2,a0
                lea     OFFSET_VERTEX_TABLE.l,a3
                moveq   #3,d7
                move.w  (a2)+,d1
                lea     0(a3,d1.w),a4
                move.l  (a4)+,(a0)+
                move.w  (a4),d6
                move.w  d6,(a0)+
                move.w  (a2)+,d1
                lea     0(a3,d1.w),a4
                move.l  (a4)+,(a0)+
                move.w  (a4),(a0)+
                and.w   (a4),d6
                move.w  (a2)+,d1
                blt.b   $C20098
                lea     0(a3,d1.w),a4
                move.l  (a4)+,(a0)+
                move.w  (a4),(a0)+
                and.w   (a4),d6
                addq.w  #1,d7
                bra.b   $C20086
                andi.w  #$7FFF,d1
                lea     0(a3,d1.w),a4
                move.l  (a4)+,(a0)+
                move.w  (a4),(a0)+
                and.w   (a4),d6
                blt.b   $C200F2
