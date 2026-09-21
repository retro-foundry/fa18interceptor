; Byte-exact selectable-missions queue builder $C1017E-$C101FB.
; Bounded key-6 trace records selector queue [$0040,$806E,$0000].

                org     $C1017E

CALL_C2FD22                       equ $C2FD22
MESSAGE_SELECTOR_SEQUENCE         equ $C4574A
SELECTABLE_RECORD_WORDS           equ $C3ED00
SELECTABLE_CONDITION_STATE_POINTER equ $C1AB74
SELECTABLE_CONDITION_OFFSET        equ $12
MENU_CALLBACK_SLOT                equ $C1820C
TOP_LEVEL_MENU_FOLLOWUP           equ $C0FCB4

build_selectable_missions_text_queue:
                link.w  a6,#-10
                jsr     CALL_C2FD22.l
                lea.l   MESSAGE_SELECTOR_SEQUENCE.l,a0
                move.w  #$40,(a0)
                addq.l  #2,a0
                move.l  #SELECTABLE_RECORD_WORDS,-8(a6)
                move.b  #3,-9(a6)
                move.l  a0,-4(a6)
.conditional_loop:
                move.b  -9(a6),d0
                cmpi.b  #9,d0
                bge.b   .append_return_instruction
                ext.w   d0
                ext.l   d0
                movea.l SELECTABLE_CONDITION_STATE_POINTER.l,a0
                ; Captured immediate form; VASM otherwise contracts it to LEA.
                dc.w    $D0FC,SELECTABLE_CONDITION_OFFSET ; adda.w #$12,a0
                adda.l  d0,a0
                tst.b   (a0)
                beq.b   .next_conditional
                movea.l -8(a6),a0
                movea.l -4(a6),a1
                move.w  (a0),(a1)
                addq.l  #2,-8(a6)
                addq.l  #2,-4(a6)
.next_conditional:
                addq.b  #1,-9(a6)
                bra.b   .conditional_loop
.append_return_instruction:
                movea.l -4(a6),a0
                move.w  #$806E,(a0)
                addq.l  #2,-4(a6)
                movea.l -4(a6),a0
                clr.w   (a0)
                lea.l   TOP_LEVEL_MENU_FOLLOWUP(pc),a0
                move.l  a0,MENU_CALLBACK_SLOT.l
                unlk    a6
                rts
