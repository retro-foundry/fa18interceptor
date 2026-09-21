; Byte-exact static-only variant-pair resolution $C33B8C-$C33C17.
                org     $C33B8C
POSTFLIGHT_STATE_WORD_B         equ $C4593A
POSTFLIGHT_VARIANT_PAIR         equ $C4593E
POSTFLIGHT_VARIANT_PAIR_SECOND  equ $C4593C
POSTFLIGHT_VARIANT_PAIR_FOURTH  equ $C45940
POSTFLIGHT_ENABLE_BYTE          equ $C458B4
POSTFLIGHT_STATE_WORD_A         equ $C459C0
POSTFLIGHT_STATUS_LONG          equ $C45B54
POSTFLIGHT_VARIANT_HELPER       equ $C31E6C
POSTFLIGHT_SUBMIT_HELPER        equ $C2F5C0
resolve_postflight_variant_pair:
                move.w  POSTFLIGHT_STATE_WORD_B.l,d0
                ble.s   postflight_variant_outside_bounds
                sub.w   POSTFLIGHT_VARIANT_PAIR.l,d0
                bge.s   postflight_variant_first_delta_ready
                neg.w   d0
postflight_variant_first_delta_ready:
                cmpi.w  #8,d0
                bgt.s   postflight_variant_outside_bounds
                move.w  POSTFLIGHT_VARIANT_PAIR_SECOND.l,d1
                sub.w   POSTFLIGHT_VARIANT_PAIR_FOURTH.l,d1
                bge.s   postflight_variant_second_delta_ready
                neg.w   d1
postflight_variant_second_delta_ready:
                cmpi.w  #8,d1
                bgt.s   postflight_variant_outside_bounds
                cmpi.w  #$900,$4A(a1)
                bgt.s   postflight_variant_outside_bounds
                tst.b   POSTFLIGHT_ENABLE_BYTE.l
                bne.s   postflight_variant_invoke_helper
                tst.w   POSTFLIGHT_STATE_WORD_A.l
                blt.s   postflight_variant_outside_bounds
                ori.l   #4,POSTFLIGHT_STATUS_LONG.l
                move.b  #1,POSTFLIGHT_ENABLE_BYTE.l
postflight_variant_invoke_helper:
                jsr     POSTFLIGHT_VARIANT_HELPER.l
                bra.s   postflight_variant_restore_pair
postflight_variant_outside_bounds:
                tst.b   POSTFLIGHT_ENABLE_BYTE.l
                beq.s   postflight_variant_restore_pair
                clr.b   POSTFLIGHT_ENABLE_BYTE.l
                ori.l   #8,POSTFLIGHT_STATUS_LONG.l
postflight_variant_restore_pair:
                movem.w POSTFLIGHT_VARIANT_PAIR.l,d0-d1
                tst.w   d0
                ble.w   $C33CC4
                jsr     POSTFLIGHT_SUBMIT_HELPER.l
