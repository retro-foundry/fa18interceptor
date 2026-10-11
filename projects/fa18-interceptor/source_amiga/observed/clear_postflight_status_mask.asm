; Byte-exact partially runtime-observed status-mask gate $C33DA4-$C33DC7.
                org     $C33DA4
POSTFLIGHT_STATUS_SOURCE        equ $C45B50
POSTFLIGHT_STATUS_FLAGS         equ $C45B54
clear_postflight_status_mask:
                move.l  POSTFLIGHT_STATUS_SOURCE.l,d5
                andi.l  #$4200,d5
                beq.s   postflight_status_mask_done
                andi.l  #$FFFFBDFF,POSTFLIGHT_STATUS_SOURCE.l
                ori.l   #8,POSTFLIGHT_STATUS_FLAGS.l
postflight_status_mask_done:
                rts
