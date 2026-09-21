; Byte-exact mid-nibble control-field routes $C1B540-$C1B58D.

                org     $C1B540

CONTROL_SELECTION_A             equ $08
CONTROL_SELECTION_B             equ $04
CONTROL_SELECTION_ZERO          equ 0
CONTROL_SELECTION_LATCH         equ $C45830
CONTROL_FIELD                   equ $C461E9
CONTROL_FIELD_BITS_32_CLEAR     equ $F3
CONTROL_GATE_A                  equ $C457AD
CONTROL_GATE_B                  equ $C457B4

SHARED_COMMAND_QUEUE            equ $C1C23C

queue_control_selection_a:
                bsr.w   select_control_selection_a
                bra.w   SHARED_COMMAND_QUEUE

queue_control_selection_b:
                bsr.w   select_control_selection_b
                bra.w   SHARED_COMMAND_QUEUE

queue_control_selection_zero:
                bsr.w   select_control_selection_zero
                bra.w   SHARED_COMMAND_QUEUE

select_control_selection_a:
                moveq   #CONTROL_SELECTION_A,d2
                bra.b   publish_control_selection

select_control_selection_b:
                moveq   #CONTROL_SELECTION_B,d2
                bra.b   publish_control_selection

select_control_selection_zero:
                moveq   #CONTROL_SELECTION_ZERO,d2
publish_control_selection:
                move.b  d2,CONTROL_SELECTION_LATCH.l
                tst.b   CONTROL_GATE_A.l
                bne.b   .merge_field
                tst.b   CONTROL_GATE_B.l
                beq.w   .done
.merge_field:
                move.b  CONTROL_FIELD.l,d1
                andi.b  #CONTROL_FIELD_BITS_32_CLEAR,d1
                or.b    d2,d1
                move.b  d1,CONTROL_FIELD.l
.done:
                rts
