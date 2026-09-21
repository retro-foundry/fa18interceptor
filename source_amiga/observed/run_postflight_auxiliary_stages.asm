; Byte-exact static-only postflight auxiliary stage entry $C332BC-$C332FB.

                org     $C332BC

POSTFLIGHT_MASK_LONG             equ $C456E6
POSTFLIGHT_FLAG                  equ $C45785
POSTFLIGHT_SECONDARY_FLAG        equ $C457A1
POSTFLIGHT_REENTRY               equ $C332B4
POSTFLIGHT_SECONDARY_SKIP        equ $C332BA
POSTFLIGHT_LOCAL_SETUP           equ $C332FE
POSTFLIGHT_STAGE_A               equ $C34146
POSTFLIGHT_STAGE_B               equ $C342D0
POSTFLIGHT_STAGE_C               equ $C33DC8
POSTFLIGHT_EXTERNAL_STAGE_A      equ $C31C60
POSTFLIGHT_EXTERNAL_STAGE_B      equ $C31D64
POSTFLIGHT_STAGE_D               equ $C33370
POSTFLIGHT_STAGE_E               equ $C33B38

run_postflight_auxiliary_stages:
                move.l  #$000FFFFF,POSTFLIGHT_MASK_LONG.l
                tst.b   POSTFLIGHT_FLAG.l
                bne.s   POSTFLIGHT_REENTRY
                bsr.w   POSTFLIGHT_LOCAL_SETUP
                bsr.w   POSTFLIGHT_STAGE_A
                bsr.w   POSTFLIGHT_STAGE_B
                bsr.w   POSTFLIGHT_STAGE_C
                jsr     POSTFLIGHT_EXTERNAL_STAGE_A.l
                tst.b   POSTFLIGHT_SECONDARY_FLAG.l
                beq.s   POSTFLIGHT_SECONDARY_SKIP
                jsr     POSTFLIGHT_EXTERNAL_STAGE_B.l
                bsr.w   POSTFLIGHT_STAGE_D
                bsr.w   POSTFLIGHT_STAGE_E
                rts
