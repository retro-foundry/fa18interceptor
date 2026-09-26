; Byte-exact activity and index-update tail $C1617E-$C16283.

                org     $C1617E

DISPLAY_VIEWPORT                equ $C1822A
OUTER_ACTIVITY_COUNTER          equ $C45899
OUTER_SECONDARY_COUNTER         equ $C458A4
OUTER_SELECTED_INDEX             equ $C4566C
OUTER_STATUS_WORD               equ $C458D2
OUTER_DYNAMIC_POINTER            equ $C45660
OUTER_STATIC_POINTER             equ $C084D0

OUTER_POINTER_SETUP              equ $C53F88
OUTER_ACTIVITY_HELPER            equ $C53F44
OUTER_POINTER_OPERATION          equ $C53EC0

run_outer_loop_child_tail:
                tst.b   OUTER_ACTIVITY_COUNTER.l
                beq.w   .secondary_counter
.activity_loop:
                pea.l   DISPLAY_VIEWPORT.l
                jsr     OUTER_POINTER_SETUP.l
                addq.l  #4,a7
                jsr     OUTER_ACTIVITY_HELPER.l
.activity_loop_test:
                move.b  OUTER_ACTIVITY_COUNTER.l,d0
                tst.b   d0
                ble.w   .finish
                moveq   #$20,d0
                move.l  d0,-(a7)
                pea.l   OUTER_STATIC_POINTER.l
                pea.l   DISPLAY_VIEWPORT.l
                jsr     OUTER_POINTER_OPERATION.l
                lea.l   $C(a7),a7
                pea.l   DISPLAY_VIEWPORT.l
                jsr     OUTER_POINTER_SETUP.l
                addq.l  #4,a7
                pea.l   DISPLAY_VIEWPORT.l
                jsr     OUTER_POINTER_SETUP.l
                addq.l  #4,a7
                moveq   #$20,d0
                move.l  d0,-(a7)
                move.l  OUTER_DYNAMIC_POINTER.l,-(a7)
                pea.l   DISPLAY_VIEWPORT.l
                jsr     OUTER_POINTER_OPERATION.l
                lea.l   $C(a7),a7
                pea.l   DISPLAY_VIEWPORT.l
                jsr     OUTER_POINTER_SETUP.l
                addq.l  #4,a7
                pea.l   DISPLAY_VIEWPORT.l
                jsr     OUTER_POINTER_SETUP.l
                addq.l  #4,a7
                move.b  OUTER_ACTIVITY_COUNTER.l,d0
                subq.b  #1,d0
                move.b  d0,OUTER_ACTIVITY_COUNTER.l
                bra.w   .activity_loop_test
.secondary_counter:
                tst.b   OUTER_SECONDARY_COUNTER.l
                beq.s   .finish
                move.b  OUTER_SECONDARY_COUNTER.l,d0
                subq.b  #1,d0
                move.b  d0,OUTER_SECONDARY_COUNTER.l
                move.w  OUTER_STATUS_WORD.l,d0
                btst    #8,d0
                bne.s   .finish
                pea.l   DISPLAY_VIEWPORT.l
                jsr     OUTER_POINTER_SETUP.l
                addq.l  #4,a7
                moveq   #$20,d0
                move.l  d0,-(a7)
                move.l  OUTER_DYNAMIC_POINTER.l,-(a7)
                pea.l   DISPLAY_VIEWPORT.l
                jsr     OUTER_POINTER_OPERATION.l
                lea.l   $C(a7),a7
.finish:
                move.w  OUTER_SELECTED_INDEX.l,d0
                moveq   #1,d1
                sub.w   d0,d1
                move.w  d1,OUTER_SELECTED_INDEX.l
                unlk    a6
                rts
