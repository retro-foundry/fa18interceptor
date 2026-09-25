; Byte-exact activity record status formatter $C328DA-$C329D9.
; Literal labels and display submission are proven; gameplay ownership is not.

                org     $C328DA

ACTIVITY_STATUS_SCRATCH         equ     $C457FA
ACTIVITY_RENDER_LANE            equ     $C45986
ACTIVITY_RENDER_OFFSET          equ     $C45918
ACTIVITY_GLYPH_RENDERER         equ     $C32794

format_activity_record_status:
                cmpi.b  #$10,d2
                beq.b   .gun_mode
                cmpi.b  #$30,d2
                beq.b   .am_mode
.sw_mode:
                move.b  #' ',(a2)
                move.b  #'S',1(a2)
                move.b  #'W',2(a2)
                lsr.b   #4,d0
                lea.l   5(a2),a0
                moveq   #4,d6
                moveq   #0,d7
                ; LEA $C3191C(PC),A1; retain original PC-relative encoding.
                dc.w    $43FA,$F01A
                bra.b   .render_status
.am_mode:
                move.b  #' ',(a2)
                move.b  #'A',1(a2)
                move.b  #'M',2(a2)
                andi.b  #$F,d0
                lea.l   5(a2),a0
                moveq   #4,d6
                moveq   #0,d7
                ; LEA $C3191C(PC),A1; retain original PC-relative encoding.
                dc.w    $43FA,$EFF8
                bra.b   .render_status
.gun_mode:
                move.w  $60(a1),d0
                move.b  #'G',(a2)
                move.b  #'U',1(a2)
                move.b  #'N',2(a2)
                lea.l   7(a2),a0
                moveq   #6,d6
                moveq   #2,d7
                ; LEA $C3191C(PC),A1; retain original PC-relative encoding.
                dc.w    $43FA,$EFD6
.render_status:
                move.b  #' ',3(a2)
                move.b  #' ',5(a2)
                move.b  #' ',6(a2)
                move.w  d7,d2
                lea.l   $1CC6.w,a4
                lea.l   $6.w,a5
                move.w  #$FCA,d6
                move.w  #4,d5
                move.w  d0,-(a7)
                moveq   #2,d0
                swap    d6
                move.w  ACTIVITY_RENDER_LANE.l,d6
                move.l  ACTIVITY_RENDER_OFFSET.l,d7
                bsr.w   ACTIVITY_GLYPH_RENDERER
                move.w  (a7)+,d0
                lea.l   ACTIVITY_STATUS_SCRATCH.l,a2
                tst.w   d0
                ble.b   .not_armed
                move.b  #'A',(a2)
                move.b  #'R',1(a2)
                move.b  #'M',2(a2)
                bra.b   .render_arm_state
.not_armed:
                move.b  #' ',(a2)
                move.b  #'N',1(a2)
                move.b  #'O',2(a2)
.render_arm_state:
                ; LEA $C3191C(PC),A1; retain original PC-relative encoding.
                dc.w    $43FA,$EF6A
                move.w  d7,d2
                lea.l   $1B86.w,a4
                lea.l   $6.w,a5
                move.w  #4,d5
                move.w  #$FCA,d6
                swap    d6
                moveq   #2,d0
                move.w  ACTIVITY_RENDER_LANE.l,d6
                move.l  ACTIVITY_RENDER_OFFSET.l,d7
                bra.w   ACTIVITY_GLYPH_RENDERER

