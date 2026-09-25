; Byte-exact message-record selector entry $C32D24-$C32E19.
; Positive selectors 74/109 and negative-form selector $806E are trace-backed.

                org     $C32D24

LAYOUT_DESCRIPTOR_BASE          equ $C41066
MESSAGE_RELATIVE_TABLE          equ $C3ED0A
NEGATIVE_LAYOUT_BASE_POINTER    equ $C45706
MESSAGE_LAYOUT_STATE_A          equ $C45744
MESSAGE_LAYOUT_STATE_B          equ $C45746
MESSAGE_LAYOUT_STATE_C          equ $C45748
MESSAGE_SEQUENCE_MODE           equ $C457E0
MESSAGE_COMMAND_BUFFER_A        equ $C457E1
MESSAGE_COMMAND_BUFFER_B        equ $C457EB
MESSAGE_COMMAND_INDEX           equ $C457F7
MESSAGE_COMMAND_CURSOR          equ $C457F8
MESSAGE_COMMAND_FLAG            equ $C457F9
MESSAGE_AUXILIARY_BYTE          equ $C457DE
MESSAGE_ACTIVE_FLAG             equ $C457C3
MESSAGE_RECORD_ATTRIBUTE        equ $C457DC
MESSAGE_RECORD_SUBTYPE          equ $C457DB
MESSAGE_LAYOUT_INDEX            equ $C45952
MESSAGE_LAYOUT_TRIPLET          equ $C4570A
CONTINUE_MESSAGE_COMPOSITOR     equ $C32F5C

select_message_record:
                lea.l   LAYOUT_DESCRIPTOR_BASE.l,a1
                lea.l   MESSAGE_RELATIVE_TABLE.l,a0
                move.w  d0,d2
                bgt.b   .resolve_record
                movea.l NEGATIVE_LAYOUT_BASE_POINTER.l,a4
                ; Captured 68000 immediate form; VASM otherwise contracts it to LEA.
                dc.w    $D9FC,$0000,$01B8         ; adda.l #$1B8,a4
                andi.w  #$3FFF,d0
.resolve_record:
                subq.w  #1,d0
                add.w   d0,d0
                move.w  (a0,d0.w),d0
                lea.l   (a0,d0.w),a2
                tst.w   d2
                bge.b   .decode_header
                addq.w  #2,a2
                bra.b   .initialize_state
.decode_header:
                moveq   #0,d0
                move.b  (a2)+,d0
                asl.w   #3,d0
                move.w  d0,d1
                asl.w   #2,d0
                add.w   d1,d0
                move.b  (a2)+,d1
                ext.w   d1
                add.w   d1,d0
                ext.l   d0
                movea.l d0,a4
.initialize_state:
                clr.w   MESSAGE_LAYOUT_STATE_A.l
                clr.w   MESSAGE_LAYOUT_STATE_B.l
                clr.w   MESSAGE_LAYOUT_STATE_C.l
                cmpi.b  #2,MESSAGE_SEQUENCE_MODE.l
                bge.b   .clear_command_state
                clr.b   MESSAGE_SEQUENCE_MODE.l
                bra.b   .set_active_flags
.clear_command_state:
                clr.b   MESSAGE_COMMAND_FLAG.l
                clr.b   MESSAGE_COMMAND_CURSOR.l
                clr.b   MESSAGE_COMMAND_INDEX.l
                lea.l   MESSAGE_COMMAND_BUFFER_A.l,a0
                lea.l   MESSAGE_COMMAND_BUFFER_B.l,a3
                moveq   #9,d0
.clear_command_loop:
                clr.b   (a0)+
                clr.b   (a3)+
                dbra    d0,.clear_command_loop
                moveq   #0,d4
                clr.b   MESSAGE_AUXILIARY_BYTE.l
                bra.b   .activate_record
.set_active_flags:
                move.b  #1,MESSAGE_AUXILIARY_BYTE.l
                move.b  #1,MESSAGE_AUXILIARY_BYTE+1.l
.activate_record:
                move.b  #1,MESSAGE_ACTIVE_FLAG.l
                move.b  (a2)+,MESSAGE_RECORD_ATTRIBUTE.l
                move.b  (a2)+,d1
                move.b  d1,d6
                andi.w  #$F0,d6
                lsr.w   #4,d6
                move.w  d6,MESSAGE_LAYOUT_INDEX.l
                btst    #14,d2
                beq.b   .publish_direct_layout
                move.b  #2,MESSAGE_RECORD_SUBTYPE.l
                bra.w   CONTINUE_MESSAGE_COMPOSITOR
.publish_direct_layout:
                andi.w  #$F,d1
                move.b  d1,MESSAGE_RECORD_SUBTYPE.l
                movem.l a1-a2/a4,MESSAGE_LAYOUT_TRIPLET.l
                bra.w   CONTINUE_MESSAGE_COMPOSITOR
