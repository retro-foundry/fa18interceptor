; Byte-exact static-only shared table passes $C335B6-$C33641.
                org     $C335B6
POSTFLIGHT_RECORD_BASE          equ $C46184
POSTFLIGHT_RECORD_OFFSET        equ $C458DE
POSTFLIGHT_WORK_BASE            equ $C457FA
POSTFLIGHT_VALUE_STORE          equ $C45B1E
POSTFLIGHT_HELPER_A             equ $C25A08
POSTFLIGHT_HELPER_B             equ $C32AA4
POSTFLIGHT_HELPER_C             equ $C32AB4
POSTFLIGHT_NEXT_CONTINUATION    equ $C337DC
run_postflight_shared_table_passes:
                lea.l   POSTFLIGHT_RECORD_BASE.l,a0
                adda.w  POSTFLIGHT_RECORD_OFFSET.l,a0
                moveq   #0,d0
                dc.w    $0828,$0007,$0000 ; btst.b #7,$0(a0)
                bne.s   postflight_shared_absolute_ready
                move.w  $6E(a0),d0
                bge.s   postflight_shared_absolute_ready
                neg.w   d0
postflight_shared_absolute_ready:
                cmpi.b  #$10,$62(a0)
                beq.w   $C33644
                lea.l   POSTFLIGHT_WORK_BASE.l,a2
                lea.l   $4(a2),a0
                moveq   #3,d6
                moveq   #3,d7
                ext.l   d0
                divu.w  #$C,d0
                ext.l   d0
                move.l  d0,POSTFLIGHT_VALUE_STORE.l
                jsr     POSTFLIGHT_HELPER_A.l
                lea.l   $C33294(pc),a1
                move.w  d7,d2
                lea.l   $D2A.w,a4
                move.w  #$A,d0
                swap    d0
                move.w  d6,d0
                jsr     POSTFLIGHT_HELPER_B.l
                move.w  #$71,d0
                lea.l   $C33642.l,a2
                lea.l   $2(a2),a0
                lea.l   $C332A8(pc),a1
                lea.l   $E44.w,a4
                move.w  #$C,d0
                swap    d0
                move.w  #1,d0
                jsr     POSTFLIGHT_HELPER_C.l
                bra.w   POSTFLIGHT_NEXT_CONTINUATION
