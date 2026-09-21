; Byte-exact signed-negative top-menu branch $C0FDD0-$C0FE35.
; Controlled digit 6 reaches the $FF -> $C1017E subroute.

                org     $C0FDD0

MENU_MODE_BYTE                  equ $C458A6
MENU_GUARD_BYTE                 equ $C4582A
MESSAGE_SELECTOR_SEQUENCE       equ $C4574A
MENU_CALLBACK_SLOT              equ $C1820C
CALL_C11312                     equ $C11312
CALL_C2FD22                     equ $C2FD22
CALL_C24E8A                     equ $C24E8A
BUILD_SELECTABLE_MISSIONS_QUEUE equ $C1017E
FALLBACK_NEGATIVE_CALLBACK      equ $C0FE36

route_negative_menu_selection:
                move.b  MENU_MODE_BYTE.l,d0
                tst.b   d0
                bpl.b   .clear_guard
                bsr.w   CALL_C11312
                jsr     CALL_C2FD22.l
                move.b  MENU_MODE_BYTE.l,d0
                cmpi.b  #$FF,d0
                bne.b   .fallback_negative_mode
                lea.l   BUILD_SELECTABLE_MISSIONS_QUEUE(pc),a0
                move.l  a0,MENU_CALLBACK_SLOT.l
                bra.b   .clear_mode
.fallback_negative_mode:
                jsr     CALL_C2FD22.l
                jsr     CALL_C24E8A.l
                movea.l -4(a6),a0
                move.w  #$57,(a0)
                addq.l  #2,-4(a6)
                movea.l -4(a6),a0
                clr.w   (a0)
                lea.l   FALLBACK_NEGATIVE_CALLBACK(pc),a0
                move.l  a0,MENU_CALLBACK_SLOT.l
.clear_mode:
                clr.b   MENU_MODE_BYTE.l
                bra.b   .return
.clear_guard:
                clr.b   MENU_GUARD_BYTE.l
.return:
                unlk    a6
                rts
