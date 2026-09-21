; Byte-exact periodic countdown helper $C11B44-$C11BAF.
; Called immediately after $C0F5F8 in the parent update sequence.

                org     $C11B44

NOTIFICATION_COUNTDOWN          equ $C45890
NOTIFICATION_CODE               equ $C4588E
NOTIFICATION_RELOAD             equ 8
NOTIFICATION_EXPIRED_CODE       equ $86
NOTIFICATION_QUARTER_CODE       equ 6
NOTIFICATION_HALF_CODE          equ 4
NOTIFICATION_QUARTER_OFFSET     equ 4
NOTIFICATION_HALF_REMAINDER     equ 2
NOTIFICATION_MODULO_MASK         equ 3

update_periodic_notification_code:
                move.b  NOTIFICATION_COUNTDOWN.l,d0
                subq.b  #1,d0
                move.b  d0,NOTIFICATION_COUNTDOWN.l
                tst.b   d0
                bgt.s   .check_quarter
                move.b  #NOTIFICATION_RELOAD,NOTIFICATION_COUNTDOWN.l
                move.b  #NOTIFICATION_EXPIRED_CODE,NOTIFICATION_CODE.l
                bra.s   .return
.check_quarter:
                move.b  NOTIFICATION_COUNTDOWN.l,d0
                subq.b  #NOTIFICATION_QUARTER_OFFSET,d0
                bne.s   .check_half
                move.b  #NOTIFICATION_QUARTER_CODE,NOTIFICATION_CODE.l
                bra.s   .return
.check_half:
                move.b  NOTIFICATION_COUNTDOWN.l,d0
                andi.b  #NOTIFICATION_MODULO_MASK,d0
                subq.b  #NOTIFICATION_HALF_REMAINDER,d0
                bne.s   .clamp_countdown
                move.b  #NOTIFICATION_HALF_CODE,NOTIFICATION_CODE.l
                bra.s   .return
.clamp_countdown:
                move.b  NOTIFICATION_COUNTDOWN.l,d0
                cmpi.b  #NOTIFICATION_RELOAD,d0
                ble.s   .return
                move.b  #NOTIFICATION_RELOAD,NOTIFICATION_COUNTDOWN.l
                clr.b   NOTIFICATION_CODE.l
.return:
                rts
