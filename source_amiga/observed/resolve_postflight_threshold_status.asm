; Byte-exact static-only threshold/status resolution $C33E66-$C33ED7.
                org     $C33E66
POSTFLIGHT_STATE_WORD_A         equ $C459C0
POSTFLIGHT_STATUS_WORD          equ $C45B46
POSTFLIGHT_ENABLE_BYTE          equ $C458B4
POSTFLIGHT_MODE_BYTE            equ $C458DB
POSTFLIGHT_STATUS_FLAGS         equ $C45B54
POSTFLIGHT_FOLLOWUP_ENTRY       equ $C33F38
POSTFLIGHT_LOCAL_FALLBACK       equ $C33F06
resolve_postflight_threshold_status:
                cmp.w   $4A(a1),d7
                ble.s   POSTFLIGHT_STATUS_CLEAR_ENTRY
                tst.w   POSTFLIGHT_STATE_WORD_A.l
                blt.s   POSTFLIGHT_STATUS_CLEAR_ENTRY
                cmpi.w  #$FD5D,POSTFLIGHT_STATUS_WORD.l
                blt.s   postflight_threshold_lower_status
                cmpi.b  #1,POSTFLIGHT_ENABLE_BYTE.l
                bne.s   postflight_threshold_set_first_status
                move.b  POSTFLIGHT_MODE_BYTE.l,d7
                andi.b  #$1F,d7
                cmpi.b  #9,d7
                beq.s   postflight_threshold_set_first_status
                bra.w   POSTFLIGHT_FOLLOWUP_ENTRY
postflight_threshold_set_first_status:
                ori.l   #4,POSTFLIGHT_STATUS_FLAGS.l
                move.b  #1,POSTFLIGHT_ENABLE_BYTE.l
                bra.w   POSTFLIGHT_FOLLOWUP_ENTRY
postflight_threshold_lower_status:
                cmpi.b  #2,POSTFLIGHT_ENABLE_BYTE.l
                beq.s   POSTFLIGHT_FOLLOWUP_ENTRY
                ori.l   #$10,POSTFLIGHT_STATUS_FLAGS.l
                move.b  #2,POSTFLIGHT_ENABLE_BYTE.l
                bra.s   POSTFLIGHT_FOLLOWUP_ENTRY
POSTFLIGHT_STATUS_CLEAR_ENTRY:
                clr.b   POSTFLIGHT_ENABLE_BYTE.l
                bra.s   POSTFLIGHT_LOCAL_FALLBACK
