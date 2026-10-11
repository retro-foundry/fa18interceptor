; Byte-exact observed first scaled delta and mirrored stores $C17DC6-$C17E09.

                org     $C17DC6

SHARED_UPDATE_PRIMARY_POINTER   equ     $C4FE3C
SHARED_UPDATE_SECONDARY_POINTER equ     $C4FE38
SHARED_UPDATE_SCALE_HELPER      equ     $C52EC8

update_c17dc6_shared_pointer_first_pair:
                movea.l SHARED_UPDATE_PRIMARY_POINTER.l,a0
                moveq   #$10,d0
                move.l  $8(a6),d1
                asl.l   d0,d1
                movea.l SHARED_UPDATE_SECONDARY_POINTER.l,a1
                sub.l   $8(a1),d1
                move.l  d1,d0
                move.l  $10(a6),d1
                jsr     SHARED_UPDATE_SCALE_HELPER.l
                move.l  d0,$18(a0)
                movea.l SHARED_UPDATE_SECONDARY_POINTER.l,a1
                move.l  d0,$18(a1)
                move.l  $10(a6),d0
                move.l  d0,$38(a0)
                movea.l SHARED_UPDATE_SECONDARY_POINTER.l,a1
                move.l  d0,$38(a1)
