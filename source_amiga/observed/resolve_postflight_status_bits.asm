; Byte-exact static-only postflight status resolution $C31722-$C3180B.

                org     $C31722

POSTFLIGHT_TABLE_TERMINATOR      equ $C4E71C
POSTFLIGHT_STATUS_BYTE           equ $C4586D
POSTFLIGHT_FLAG_BYTE             equ $C4586E
POSTFLIGHT_STATUS_TIMER          equ $C45887
POSTFLIGHT_MODE_BYTE             equ $C458A6
POSTFLIGHT_STATUS_WORD           equ $C45AE0
POSTFLIGHT_EVENT_WORD            equ $C45B54
POSTFLIGHT_SCAN_REQUEST          equ $C457B9

resolve_postflight_status_bits:
                move.w  #-$1,(a2)
                move.b  POSTFLIGHT_STATUS_BYTE.l,d0
                beq.w   postflight_status_special_word
                move.b  POSTFLIGHT_FLAG_BYTE.l,d1
                btst    #4,d0
                bne.s   postflight_status_bit_four
                btst    #5,d0
                beq.s   postflight_status_bit_three
                btst    #5,d1
                bne.w   postflight_status_publish
                ori.l   #1,POSTFLIGHT_EVENT_WORD.l
                bra.w   postflight_status_publish
postflight_status_bit_four:
                btst    #4,d1
                bne.w   postflight_status_publish
                ori.l   #1,POSTFLIGHT_EVENT_WORD.l
                bra.w   postflight_status_publish
postflight_status_bit_three:
                btst    #3,d0
                beq.s   postflight_status_bit_two_without_three
                btst    #3,d1
                bne.s   postflight_status_bit_two_with_three
                ori.l   #2,POSTFLIGHT_EVENT_WORD.l
                bra.s   postflight_status_set_timer_value
postflight_status_bit_two_with_three:
                btst    #2,d0
                beq.s   postflight_status_bit_one
                btst    #2,d1
                beq.s   postflight_status_set_event_two
                bra.w   postflight_status_publish
postflight_status_bit_two_without_three:
                btst    #2,d0
                beq.s   postflight_status_bit_one_without_bit_two
                btst    #2,d1
                bne.s   postflight_status_bit_one
postflight_status_set_event_two:
                ori.l   #2,POSTFLIGHT_EVENT_WORD.l
postflight_status_set_timer_value:
                move.b  #$18,POSTFLIGHT_STATUS_TIMER.l
                bra.s   postflight_status_publish
postflight_status_bit_one:
                btst    #1,d0
                beq.s   postflight_status_publish
                btst    #1,d1
                bne.s   postflight_status_publish
                cmpi.b  #5,POSTFLIGHT_MODE_BYTE.l
                beq.s   postflight_status_publish
                bra.s   postflight_status_set_event_800
postflight_status_bit_one_without_bit_two:
                btst    #1,d0
                beq.s   postflight_status_special_word
                cmpi.b  #5,POSTFLIGHT_MODE_BYTE.l
                beq.s   postflight_status_special_word
                btst    #1,d1
                bne.s   postflight_status_publish
postflight_status_set_event_800:
                ori.l   #$800,POSTFLIGHT_EVENT_WORD.l
                bra.s   postflight_status_publish
postflight_status_special_word:
                move.w  POSTFLIGHT_STATUS_WORD.l,d1
                andi.w  #$FF,d1
                cmpi.w  #$800E,d1
                bne.s   postflight_status_publish
                clr.w   POSTFLIGHT_STATUS_WORD.l
postflight_status_publish:
                move.b  d0,POSTFLIGHT_FLAG_BYTE.l
