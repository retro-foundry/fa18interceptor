; Byte-exact observed table-record copy path $C1FFB4-$C1FFFD.
; The negative branch destination is outside this observed path.

                org     $C1FFB4

C1FFFE                         equ     $C1FFFE
C2EE4A                         equ     $C2EE4A
C4C592                         equ     $C4C592
C48390                         equ     $C48390
C456E6                         equ     $C456E6
C45954                         equ     $C45954

copy_c1ffb4_table_record:
                lea     C2EE4A.l,a4
                move.l  #-1,C456E6.l
                lea     C4C592.l,a0
                lea     C48390.l,a3
                move.w  (a2)+,d1
                move.l  (a3,d1.w),(a0)+
                move.w  4(a3,d1.w),d6
                move.w  d6,(a0)+
                move.w  (a2)+,d1
                move.l  (a3,d1.w),(a0)+
                move.w  4(a3,d1.w),d7
                move.w  (a2)+,C45954.l
                and.w   d7,d6
                blt.b   C1FFFE
                move.w  d7,(a0)+
                movem.l a1/a2/a5,-(sp)
                jsr     (a4)
                movem.l (sp)+,a1/a2/a5
                rts
