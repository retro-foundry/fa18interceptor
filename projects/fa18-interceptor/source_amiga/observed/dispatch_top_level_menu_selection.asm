; Byte-exact top-menu callback head $C0FCB4-$C0FDCF.
; The companion signed-negative tail begins at $C0FDD0.

                org     $C0FCB4

MESSAGE_SELECTOR_SEQUENCE       equ $C4574A
LAST_MESSAGE_SELECTOR            equ $C45772
MENU_MODE_BYTE                  equ $C458A6
MENU_GUARD_BYTE                 equ $C4582A
MENU_VIDEO_AUXILIARY            equ $C4582B
MENU_ADVANCED_MARKER            equ $C45792
MENU_DISPLAY_MODE               equ $C45668
MENU_DISPLAY_DELAY              equ $C45AD6
MENU_VIDEO_FLAGS                equ $C45B5B
MENU_CALLBACK_SLOT              equ $C1820C
CALL_C11312                     equ $C11312
CALL_C2FD22                     equ $C2FD22
QUEUE_TOP_LEVEL_MENU            equ $C0FBE0
ADVANCE_POSITIVE_MENU_MODE      equ $C0FECE
RETURN_TOP_MENU_CALLBACK        equ $C0FE32

dispatch_top_level_menu_selection:
                link.w  a6,#-6
                move.l  #MESSAGE_SELECTOR_SEQUENCE,-4(a6)
                move.b  MENU_MODE_BYTE.l,-5(a6)
                tst.b   MENU_GUARD_BYTE.l
                beq.b   .test_menu_mode
                clr.b   MENU_GUARD_BYTE.l
                move.w  LAST_MESSAGE_SELECTOR.l,d0
                subq.w  #6,d0
                beq.w   RETURN_TOP_MENU_CALLBACK
                bsr.w   CALL_C11312
                lea.l   QUEUE_TOP_LEVEL_MENU(pc),a0
                move.l  a0,MENU_CALLBACK_SLOT.l
                bra.w   RETURN_TOP_MENU_CALLBACK
.test_menu_mode:
                move.b  MENU_MODE_BYTE.l,d0
                tst.b   d0
                ble.w   .negative_menu_mode
                clr.l   MENU_DISPLAY_MODE.l
                bsr.w   CALL_C11312
                jsr     CALL_C2FD22.l
                cmpi.b  #$7F,-5(a6)
                bne.b   .test_free_flight
                movea.l -4(a6),a0
                move.w  #101,(a0)
                addq.l  #2,-4(a6)
                bra.b   .finish_positive_selection
.test_free_flight:
                cmpi.b  #1,-5(a6)
                bne.b   .test_training_demo
                movea.l -4(a6),a0
                move.w  #102,(a0)
                addq.l  #2,-4(a6)
                bra.b   .finish_positive_selection
.test_training_demo:
                cmpi.b  #2,-5(a6)
                bne.b   .test_training_practice
                movea.l -4(a6),a0
                move.w  #103,(a0)
                addq.l  #2,-4(a6)
                bra.b   .finish_positive_selection
.test_training_practice:
                cmpi.b  #$7D,-5(a6)
                bne.b   .test_qualification
                movea.l -4(a6),a0
                move.w  #104,(a0)
                addq.l  #2,-4(a6)
                bra.b   .finish_positive_selection
.test_qualification:
                cmpi.b  #9,-5(a6)
                bne.b   .test_advanced_marker
                movea.l -4(a6),a0
                move.w  #105,(a0)
                addq.l  #2,-4(a6)
                bra.b   .finish_positive_selection
.test_advanced_marker:
                tst.b   MENU_ADVANCED_MARKER.l
                beq.b   .finish_positive_selection
                movea.l -4(a6),a0
                move.w  #107,(a0)
                addq.l  #2,-4(a6)
                clr.b   MENU_ADVANCED_MARKER.l
.finish_positive_selection:
                movea.l -4(a6),a0
                clr.w   (a0)
                move.b  MENU_VIDEO_AUXILIARY.l,d0
                tst.b   d0
                bne.b   .set_standard_delay
                btst.b  #7,MENU_VIDEO_FLAGS.l
                beq.b   .set_standard_delay
                move.w  #$D2,MENU_DISPLAY_DELAY.l
                bra.b   .install_positive_callback
.set_standard_delay:
                move.w  #$96,MENU_DISPLAY_DELAY.l
.install_positive_callback:
                lea.l   ADVANCE_POSITIVE_MENU_MODE(pc),a0
                move.l  a0,MENU_CALLBACK_SLOT.l
                bra.b   RETURN_TOP_MENU_CALLBACK
.negative_menu_mode:
                ; External continuation: $C0FDD0-$C0FE35.
