; Byte-exact partially runtime-observed secondary variant gate $C33DC8-$C33DFD.
                org     $C33DC8
POSTFLIGHT_STATE_WORD_A         equ $C459C0
POSTFLIGHT_RECORD_BASE          equ $C46184
POSTFLIGHT_RECORD_OFFSET        equ $C458DE
POSTFLIGHT_VARIANT_PAIR         equ $C45942
POSTFLIGHT_STATUS_MASK_GATE     equ $C33DA4
POSTFLIGHT_SMALL_VALUE_ENTRY    equ $C33ED8
gate_postflight_secondary_variant:
                tst.w   POSTFLIGHT_STATE_WORD_A.l
                blt.s   POSTFLIGHT_STATUS_MASK_GATE
                lea.l   POSTFLIGHT_RECORD_BASE.l,a1
                adda.w  POSTFLIGHT_RECORD_OFFSET.l,a1
                move.b  $63(a1),d5
                andi.b  #$F0,d5
                cmpi.b  #$10,d5
                beq.s   POSTFLIGHT_STATUS_MASK_GATE
                movem.w POSTFLIGHT_VARIANT_PAIR.l,d0-d1
                tst.w   d0
                blt.s   POSTFLIGHT_STATUS_MASK_GATE
                cmpi.w  #$60,d0
                ble.w   POSTFLIGHT_SMALL_VALUE_ENTRY
