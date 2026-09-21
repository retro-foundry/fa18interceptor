; Byte-exact static-only postflight table-pass configuration $C333B2-$C33417.

                org     $C333B2

POSTFLIGHT_VARIANT_ENTRY         equ $C3341A
POSTFLIGHT_FINAL_CONTINUATION    equ $C335B6
POSTFLIGHT_VALUE_STORE           equ $C45B1E
POSTFLIGHT_WORK_BASE             equ $C457FA
POSTFLIGHT_HELPER_A              equ $C25A08
POSTFLIGHT_HELPER_B              equ $C32AA4
POSTFLIGHT_HELPER_C              equ $C32AB4

configure_postflight_table_passes:
                cmpi.b  #$10,$62(a0)
                beq.w   POSTFLIGHT_VARIANT_ENTRY
                lea     POSTFLIGHT_WORK_BASE.l,a2
                lea     $6(a2),a0
                moveq   #5,d6
                moveq   #5,d7
                move.l  d0,POSTFLIGHT_VALUE_STORE.l
                jsr     POSTFLIGHT_HELPER_A.l
                lea     $C33270(pc),a1
                move.w  d7,d2
                lea     $D38.w,a4
                move.w  #$18,d0
                swap    d0
                move.w  d6,d0
                jsr     POSTFLIGHT_HELPER_B.l
                move.w  #$CA,d0
                lea     $C33418.l,a2
                lea     $2(a2),a0
                lea     $C3328C(pc),a1
                lea     $E52.w,a4
                move.w  #$1A,d0
                swap    d0
                move.w  #1,d0
                jsr     POSTFLIGHT_HELPER_C.l
                bra.w   POSTFLIGHT_FINAL_CONTINUATION
