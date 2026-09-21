; Byte-exact raw keyboard-event poll $C16C56-$C16CD7.
; Called by the observed $C0F43A keyboard drain.

                org     $C16C56

PENDING_KEY_EVENT_FLAG          equ $C08182
PENDING_KEY_EVENT_VALUE         equ $C1ABC8
POLL_KEY_SOURCE                 equ $C16BF2
NO_RAW_KEY_EVENT                equ $00FF
RAW_KEY_RELEASE_BIT             equ $80
RAW_KEY_VALUE_MASK              equ $7F
RAW_KEY_SYSTEM_MASK             equ $70
RAW_KEY_SYSTEM_VALUE            equ $70

poll_raw_keyboard_event:
                link.w  a6,#-4
                tst.w   PENDING_KEY_EVENT_FLAG.l
                beq.b   poll_keyboard_source
                clr.w   PENDING_KEY_EVENT_FLAG.l
                move.w  PENDING_KEY_EVENT_VALUE.l,d0
                unlk    a6
                rts

poll_keyboard_source:
                bsr.w   POLL_KEY_SOURCE
                move.b  d0,-2(a6)
                tst.b   d0
                bne.b   normalize_raw_keyboard_event
                move.w  #NO_RAW_KEY_EVENT,d0
                unlk    a6
                rts

normalize_raw_keyboard_event:
                move.b  -2(a6),d0
                andi.b  #RAW_KEY_VALUE_MASK,d0
                move.b  d0,-1(a6)
                andi.b  #RAW_KEY_SYSTEM_MASK,d0
                cmpi.b  #RAW_KEY_SYSTEM_VALUE,d0
                bne.b   classify_raw_key_edge
                move.w  #NO_RAW_KEY_EVENT,d0
                unlk    a6
                rts

classify_raw_key_edge:
                move.b  -2(a6),d0
                andi.b  #RAW_KEY_RELEASE_BIT,d0
                seq.b   d1
                neg.b   d1
                ext.w   d1
                ext.l   d1
                move.w  d1,-4(a6)
                tst.w   d1
                bne.b   return_normalized_raw_key
                move.b  -1(a6),d0
                ext.w   d0
                ext.l   d0
                addi.l  #RAW_KEY_RELEASE_BIT,d0
                move.b  d0,-1(a6)

return_normalized_raw_key:
                move.b  -1(a6),d0
                ext.w   d0
                unlk    a6
                rts
