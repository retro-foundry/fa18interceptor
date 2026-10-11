; Byte-exact observed setup prefix $C1612C-$C1617D.
; This prefix is part of the complete no-input C1612C -> C15DB2 packet.

                org     $C1612C

DISPLAY_VIEWPORT                equ $C1822A
OUTER_SELECTED_INDEX             equ $C4566C
OUTER_POINTER_TABLE_1            equ $C182BA
OUTER_POINTER_TABLE_2            equ $C182C2
OUTER_SELECTED_POINTER_1         equ $C1821C
OUTER_SELECTED_POINTER_2         equ $C18232
OUTER_POINTER_UPDATE_TARGET      equ $C18218
OUTER_ACTIVITY_FLAG              equ $C45899

OUTER_POINTER_SETUP_1            equ $C53F88
OUTER_POINTER_SETUP_2            equ $C53F30

prepare_outer_loop_child:
                link.w  a6,#-2
                pea.l   DISPLAY_VIEWPORT.l
                jsr     OUTER_POINTER_SETUP_1.l
                addq.l  #4,a7
                move.w  OUTER_SELECTED_INDEX.l,d0
                move.w  d0,0.w(a7)
                ext.l   d0
                asl.l   #2,d0
                movea.l d0,a0
                adda.l  #OUTER_POINTER_TABLE_1,a0
                move.l  (a0),OUTER_SELECTED_POINTER_1.l
                move.w  0.w(a7),d0
                ext.l   d0
                asl.l   #2,d0
                movea.l d0,a0
                adda.l  #OUTER_POINTER_TABLE_2,a0
                move.l  (a0),OUTER_SELECTED_POINTER_2.l
                pea.l   OUTER_POINTER_UPDATE_TARGET.l
                jsr     OUTER_POINTER_SETUP_2.l
                addq.l  #4,a7
