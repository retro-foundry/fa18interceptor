; Byte-exact top-level menu text-sequence writer at $C0FBE0-$C0FCB3.
; The observed flight-return snapshot contains this exact resulting sequence.

                org     $C0FBE0

MESSAGE_SELECTOR_SEQUENCE        equ $C4574A
MENU_CALLBACK_SLOT               equ $C1820C
MENU_AUXILIARY_LATCH             equ $C457D7
MENU_MODE_LATCH                  equ $C458AD
MENU_DISPLAY_DELAY               equ $C4573E
MENU_VIDEO_FLAG                  equ $C45B5B
MENU_VIDEO_LATCH                 equ $C4FF26
MENU_DISPLAY_MODE                equ $C45668

CALL_C11312                      equ $C11312
CALL_C17B96                      equ $C17B96
CALL_C0E78A                      equ $C0E78A
CALL_C11ACC                      equ $C11ACC
CALL_C2FD22                      equ $C2FD22

queue_top_level_menu_text:
                link.w  a6,#-4
                move.l  #MESSAGE_SELECTOR_SEQUENCE,-4(a6)
                move.l  #$1F0000,MENU_DISPLAY_MODE.l
                btst.b  #7,MENU_VIDEO_FLAG.l
                beq.b   skip_menu_video_latch
                move.b  MENU_AUXILIARY_LATCH.l,d0
                tst.b   d0
                bne.b   skip_menu_video_latch
                move.l  #$1F0000,MENU_VIDEO_LATCH.l

skip_menu_video_latch:
                moveq   #$F,d0
                move.l  d0,-(a7)
                jsr     CALL_C17B96.l
                addq.l  #4,a7
                move.b  #1,MENU_AUXILIARY_LATCH.l
                jsr     CALL_C2FD22.l
                bsr.w   CALL_C11312
                move.l  #$C000,-(a7)
                jsr     CALL_C0E78A.l
                addq.l  #4,a7
                clr.b   MENU_MODE_LATCH.l
                pea.l   $C08490.l
                jsr     CALL_C11ACC.l
                addq.l  #4,a7
                move.l  #$1E0,MENU_DISPLAY_DELAY.l
                movea.l -4(a6),a0
                move.w  #6,(a0)
                addq.l  #2,a0
                move.w  #100,(a0)
                addq.l  #2,a0
                move.w  #101,(a0)
                addq.l  #2,a0
                move.w  #102,(a0)
                addq.l  #2,a0
                move.w  #103,(a0)
                addq.l  #2,a0
                move.w  #104,(a0)
                addq.l  #2,a0
                move.w  #105,(a0)
                addq.l  #2,a0
                move.w  #106,(a0)
                addq.l  #2,a0
                move.w  #107,(a0)
                addq.l  #2,a0
                move.w  #108,(a0)
                addq.l  #2,a0
                move.w  #109,(a0)
                addq.l  #2,a0
                clr.w   (a0)
                lea.l   top_level_menu_followup(pc),a1
                move.l  a1,MENU_CALLBACK_SLOT.l
                unlk    a6
                rts

top_level_menu_followup          equ $C0FCB4
