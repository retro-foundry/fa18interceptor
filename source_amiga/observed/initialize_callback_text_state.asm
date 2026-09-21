; Byte-exact callback installed by $C0F4D8 at $C0F812-$C0F91F.

                org     $C0F812

CALL_C08F26                     equ $C08F26
CALL_C17B96                     equ $C17B96
CALLBACK_COPY_DESTINATION        equ $C45660
CALLBACK_TABLE                   equ $C08510
CALLBACK_INPUT_WORD_ZERO         equ $C4564C
CALLBACK_INPUT_WORD_ONE          equ $C45650
CALLBACK_INPUT_WORD_TWO          equ $C45654
CALLBACK_TEXT_BUFFER             equ $C3F040
CALLBACK_ENABLE                  equ $C458A6
CALLBACK_FLAG_A                  equ $C457AD
CALLBACK_FLAG_B                  equ $C45857
CALLBACK_COUNTDOWN               equ $C45AD6
CALLBACK_SLOT                    equ $C1820C
CALLBACK_NEXT_HANDLER            equ $C11446
CALLBACK_FORMAT_HANDLER          equ $C113E4

CALLBACK_TABLE_WORD_COUNT        equ $20
CALLBACK_INPUT_ZERO_SENTINEL     equ $C560
CALLBACK_INPUT_ONE_SENTINEL      equ $7E70
CALLBACK_INPUT_TWO_SENTINEL      equ $4DE8
CALLBACK_TEXT_INITIAL_BYTE       equ $31
CALLBACK_TEXT_FLAG_ONE           equ $32
CALLBACK_TEXT_FLAG_TWO           equ $34
CALLBACK_SHORT_COUNTDOWN         equ 3

initialize_callback_text_state:
                link.w  a6,#-14
                jsr     CALL_C08F26.l
                move.l  CALLBACK_COPY_DESTINATION.l,-4(a6)
                clr.w   -10(a6)

copy_callback_table_word:
                move.w  -10(a6),d0
                cmpi.w  #CALLBACK_TABLE_WORD_COUNT,d0
                bge.b   finish_callback_table_copy
                ext.l   d0
                ; VASM folds ASL #1 to ADD; retain the original ASL encoding.
                dc.w    $E380
                movea.l d0,a0
                adda.l  #CALLBACK_TABLE,a0
                movea.l -4(a6),a1
                move.w  (a0),(a1)
                addq.l  #2,-4(a6)
                addq.w  #1,-10(a6)
                bra.b   copy_callback_table_word

finish_callback_table_copy:
                clr.b   CALLBACK_ENABLE.l
                moveq   #$FF,d0
                move.b  d0,CALLBACK_FLAG_A.l
                move.b  d0,CALLBACK_FLAG_B.l
                move.w  #CALLBACK_SHORT_COUNTDOWN,CALLBACK_COUNTDOWN.l
                move.l  #CALLBACK_NEXT_HANDLER,CALLBACK_SLOT.l
                lea.l   CALLBACK_TEXT_BUFFER.l,a0
                move.l  a0,-14(a6)
                ; VASM folds ADDA immediate to LEA; retain the original ADDA encoding.
                dc.w    $D0FC,$0015
                clr.l   -8(a6)
                moveq   #0,d0
                move.w  CALLBACK_INPUT_WORD_ZERO.l,d0
                move.l  a0,-14(a6)
                cmpi.l  #CALLBACK_INPUT_ZERO_SENTINEL,d0
                beq.b   check_callback_input_one
                move.b  #CALLBACK_TEXT_INITIAL_BYTE,(a0)
                move.l  d0,-8(a6)

check_callback_input_one:
                moveq   #0,d0
                move.w  CALLBACK_INPUT_WORD_ONE.l,d0
                cmpi.l  #CALLBACK_INPUT_ONE_SENTINEL,d0
                beq.b   check_callback_input_two
                movea.l -14(a6),a0
                move.b  (a0),d1
                ori.b   #CALLBACK_TEXT_FLAG_ONE,d1
                move.b  d1,(a0)
                move.l  d0,-8(a6)

check_callback_input_two:
                moveq   #0,d0
                move.w  CALLBACK_INPUT_WORD_TWO.l,d0
                cmpi.l  #CALLBACK_INPUT_TWO_SENTINEL,d0
                beq.b   format_callback_input
                movea.l -14(a6),a0
                move.b  (a0),d1
                ori.b   #CALLBACK_TEXT_FLAG_TWO,d1
                move.b  d1,(a0)
                move.l  d0,-8(a6)

format_callback_input:
                tst.l   -8(a6)
                beq.b   invoke_callback_fallback
                movea.l -14(a6),a0
                addq.l  #1,a0
                moveq   #4,d0
                move.l  d0,-(a7)
                move.l  -8(a6),-(a7)
                move.l  a0,-(a7)
                move.l  a0,-14(a6)
                bsr.w   format_fixed_width_hex_ascii
                lea.l   12(a7),a7
                move.l  #CALLBACK_FORMAT_HANDLER,CALLBACK_SLOT.l
                bra.b   finish_callback_text_state

invoke_callback_fallback:
                moveq   #CALLBACK_TEXT_FLAG_ONE,d0
                move.l  d0,-(a7)
                jsr     CALL_C17B96.l
                addq.l  #4,a7

finish_callback_text_state:
                unlk    a6
                rts

format_fixed_width_hex_ascii     equ $C0F56A
