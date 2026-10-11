; Byte-exact static-only variant-path gate $C33B36-$C33B7F.
                org     $C33B36
POSTFLIGHT_GATE_WORD            equ $C45A42
POSTFLIGHT_RECORD_BASE          equ $C46184
POSTFLIGHT_RECORD_OFFSET        equ $C458DE
POSTFLIGHT_STATE_WORD_A         equ $C459C0
POSTFLIGHT_STATE_WORD_B         equ $C4593A
POSTFLIGHT_VARIANT_ENTRY        equ $C33B8C
POSTFLIGHT_NEGATIVE_ENTRY       equ $C33CC4
POSTFLIGHT_NONNEGATIVE_ENTRY    equ $C33C18
postflight_variant_return:
                rts
gate_postflight_variant_path:
                cmpi.w  #$80,POSTFLIGHT_GATE_WORD.l
                bne.s   postflight_variant_return
                lea.l   POSTFLIGHT_RECORD_BASE.l,a1
                adda.w  POSTFLIGHT_RECORD_OFFSET.l,a1
                move.b  $63(a1),d5
                andi.b  #$F0,d5
                cmpi.b  #$10,d5
                beq.s   POSTFLIGHT_VARIANT_ENTRY
                tst.w   POSTFLIGHT_STATE_WORD_A.l
                blt.w   POSTFLIGHT_NEGATIVE_ENTRY
                move.w  #$9F,d0
                move.w  #$5B,d1
                tst.w   POSTFLIGHT_STATE_WORD_B.l
                ble.s   $C33B80
                tst.w   POSTFLIGHT_STATE_WORD_A.l
                bge.w   POSTFLIGHT_NONNEGATIVE_ENTRY
