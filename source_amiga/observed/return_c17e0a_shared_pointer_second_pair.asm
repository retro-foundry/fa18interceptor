; Byte-exact observed second scaled delta, mirrored stores, and return
; $C17E0A-$C17E49.

                org     $C17E0A

SHARED_UPDATE_SECONDARY_POINTER equ     $C4FE38
SHARED_UPDATE_SCALE_HELPER      equ     $C52EC8

return_c17e0a_shared_pointer_second_pair:
                moveq   #$10,d1
                move.l  $c(a6),d2
                asl.l   d1,d2
                sub.l   $c(a1),d2
                move.l  d2,d0
                move.l  $10(a6),d1
                jsr     SHARED_UPDATE_SCALE_HELPER.l
                move.l  d0,$1c(a0)
                movea.l SHARED_UPDATE_SECONDARY_POINTER.l,a1
                move.l  d0,$1c(a1)
                move.l  $10(a6),d0
                move.l  d0,$3c(a0)
                movea.l SHARED_UPDATE_SECONDARY_POINTER.l,a1
                move.l  d0,$3c(a1)
                movem.l (sp)+,d2
                unlk    a6
                rts
