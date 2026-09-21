; Byte-exact tail of parent update routine $C0F2A8-$C0F3C3.

                org     $C0F2A8

POSTFLIGHT_FLAG                 equ $C45785
POSTFLIGHT_LATCH                equ $C457D9
POSTFLIGHT_MODE                 equ $C458A6
ACTIVITY_MODE                   equ $C458AE
FLIGHT_STATE_BYTE               equ $C457AE
FRAME_COUNTER_WORD              equ $C458DA
UPDATE_STAGE_MARKER             equ $C45AD4
TAIL_CONDITION_BYTE_1           equ $C457B2
TAIL_CONDITION_BYTE_2           equ $C45795
MARKER_TAIL_START               equ $1D4
MARKER_TAIL_CONDITION_1         equ $210
MARKER_TAIL_CONDITION_2         equ $218
MARKER_TAIL_END                 equ $220

TAIL_HELPER_1                   equ $C31F4C
TAIL_HELPER_2                   equ $C3201A
TAIL_HELPER_3                   equ $C31EB6
TAIL_HELPER_4                   equ $C322EE
TAIL_HELPER_5                   equ $C12242
TAIL_HELPER_6                   equ $C31F4A
TAIL_HELPER_7                   equ $C2B564
TAIL_HELPER_8                   equ $C25312
TAIL_HELPER_9                   equ $C2548A
TAIL_HELPER_10                  equ $C53F9C
TAIL_HELPER_11                  equ $C2F582
TAIL_HELPER_12                  equ $C082B8
TAIL_HELPER_13                  equ $C12950
TAIL_HELPER_14                  equ $C2B3C2
TAIL_HELPER_15                  equ $C2F49C
TAIL_HELPER_16                  equ $C31B76
TAIL_HELPER_17                  equ $C32CEE

run_parent_update_tail:
                tst.b   POSTFLIGHT_FLAG.l
                beq.s   .tail_join
                move.b  #1,POSTFLIGHT_LATCH.l
                jsr     TAIL_HELPER_1.l
                jsr     TAIL_HELPER_2.l
                jsr     TAIL_HELPER_3.l
                nop
                move.b  POSTFLIGHT_MODE.l,d0
                subq.b  #2,d0
                bne.s   .tail_join
                jsr     TAIL_HELPER_4.l
.tail_join:
                move.w  #MARKER_TAIL_START,UPDATE_STAGE_MARKER.l
                jsr     TAIL_HELPER_5.l
                jsr     TAIL_HELPER_6.l
                jsr     TAIL_HELPER_7.l
                jsr     TAIL_HELPER_8.l
                move.w  -2(a6),d0
                andi.w  #7,d0
                subq.w  #7,d0
                bne.s   .after_modulo_7
                jsr     TAIL_HELPER_9.l
.after_modulo_7:
                move.w  -2(a6),d0
                andi.w  #$F,d0
                subq.w  #4,d0
                bne.s   .after_modulo_15
                clr.l   -(a7)
                jsr     TAIL_HELPER_10.l
                addq.l  #4,a7
.after_modulo_15:
                move.w  -2(a6),d0
                andi.w  #$1F,d0
                subq.w  #8,d0
                bne.s   .check_activity_mode
                jsr     TAIL_HELPER_11.l
                bra.s   .update_frame_counter
.check_activity_mode:
                move.b  ACTIVITY_MODE.l,d0
                tst.b   d0
                bne.s   .update_frame_counter
                move.w  -2(a6),d0
                andi.w  #$1F,d0
                cmpi.w  #$10,d0
                bne.s   .update_frame_counter
                jsr     TAIL_HELPER_12.l
.update_frame_counter:
                move.b  FLIGHT_STATE_BYTE.l,d0
                tst.b   d0
                bne.s   .run_tail_helper_14
                move.w  FRAME_COUNTER_WORD.l,d0
                addq.w  #1,d0
                move.w  d0,FRAME_COUNTER_WORD.l
                bra.s   .run_tail_helper_14
.skip_path_tail:
                jsr     TAIL_HELPER_13.l
                clr.l   -(a7)
                jsr     TAIL_HELPER_10.l
                addq.l  #4,a7
.run_tail_helper_14:
                jsr     TAIL_HELPER_14.l
                tst.b   TAIL_CONDITION_BYTE_1.l
                beq.s   .tail_end_marker
                tst.b   TAIL_CONDITION_BYTE_2.l
                beq.s   .tail_end_marker
                move.w  #MARKER_TAIL_CONDITION_1,UPDATE_STAGE_MARKER.l
                jsr     TAIL_HELPER_15.l
                move.w  #MARKER_TAIL_CONDITION_2,UPDATE_STAGE_MARKER.l
                jsr     TAIL_HELPER_16.l
.tail_end_marker:
                move.w  #MARKER_TAIL_END,UPDATE_STAGE_MARKER.l
                jsr     TAIL_HELPER_17.l
                unlk    a6
                rts
