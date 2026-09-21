; Byte-exact control-record stream advance $C1B27E-$C1B33F.

                org     $C1B27E

CONTROL_ACTIVITY_WORD           equ $C459B6
CONTROL_MODE                    equ $C4584B
CONTROL_MODE_MAX                equ 3
CONTROL_MODE_STREAM_WORD_SKIP   equ 1
CONTROL_RECORD_BYTE_POINTER     equ $C4FDBC
CONTROL_RECORD_WORD_POINTER     equ $C4FDB8
CONTROL_RECORD_WORD_BASE        equ $C4FDB0
CONTROL_RECORD_BYTE_BASE        equ $C4FDA4
CONTROL_RECORD_BYTE_END         equ $C4FDAC
CONTROL_RECORD_WORD_COUNT       equ $C4FDA8
CONTROL_RECORD_BYTE_FIELD       equ $C461E9
CONTROL_RECORD_WORD_FIELD_A     equ $C45996
CONTROL_RECORD_WORD_FIELD_B     equ $C45998
STREAM_SENTINEL                 equ $FF

UPDATE_CONTROL_FIELD_FROM_DELTA equ $C1B340
GATE_THREE_AXIS_CONTROL_UPDATE  equ $C1B3EA
REFRESH_BUFFERED_COMMAND_STATE  equ $C1C1B4

advance_control_record_stream:
                tst.w   CONTROL_ACTIVITY_WORD.l
                bne.w   GATE_THREE_AXIS_CONTROL_UPDATE
                move.b  CONTROL_MODE.l,d0
                ble.w   UPDATE_CONTROL_FIELD_FROM_DELTA
                cmpi.b  #CONTROL_MODE_MAX,d0
                bgt.w   UPDATE_CONTROL_FIELD_FROM_DELTA

                movea.l CONTROL_RECORD_BYTE_POINTER.l,a0
                move.b  (a0)+,CONTROL_RECORD_BYTE_FIELD.l
                move.l  a0,CONTROL_RECORD_BYTE_POINTER.l
                movea.l CONTROL_RECORD_WORD_POINTER.l,a2
                move.w  (a2)+,CONTROL_RECORD_WORD_FIELD_A.l
                move.w  (a2)+,d0
                cmpi.b  #CONTROL_MODE_STREAM_WORD_SKIP,CONTROL_MODE.l
                beq.b   .advance_word_pointer
                move.w  d0,CONTROL_RECORD_WORD_FIELD_B.l
.advance_word_pointer:
                move.l  CONTROL_RECORD_WORD_COUNT.l,d0
                add.l   d0,d0
                add.l   d0,d0
                add.l   CONTROL_RECORD_WORD_BASE.l,d0
                cmp.l   a2,d0
                bgt.b   .store_word_pointer
                movea.l CONTROL_RECORD_WORD_BASE.l,a2
.store_word_pointer:
                move.l  a2,CONTROL_RECORD_WORD_POINTER.l

.check_sentinel:
                movea.l CONTROL_RECORD_BYTE_POINTER.l,a3
                cmpi.b  #STREAM_SENTINEL,(a3)+
                bne.b   .check_byte_end
                cmpi.b  #STREAM_SENTINEL,(a3)+
                bne.b   .check_byte_end
                cmpi.b  #STREAM_SENTINEL,(a3)+
                beq.b   .refresh_buffered_state
.check_byte_end:
                cmpa.l  CONTROL_RECORD_BYTE_END.l,a0
                beq.b   .refresh_buffered_state
                move.l  CONTROL_RECORD_BYTE_BASE.l,d0
                add.l   CONTROL_RECORD_WORD_COUNT.l,d0
                cmp.l   a0,d0
                bgt.b   UPDATE_CONTROL_FIELD_FROM_DELTA
                move.l  CONTROL_RECORD_BYTE_BASE.l,CONTROL_RECORD_BYTE_POINTER.l
                move.l  CONTROL_RECORD_WORD_BASE.l,CONTROL_RECORD_WORD_POINTER.l
                addq.l  #4,CONTROL_RECORD_WORD_POINTER.l
                movea.l CONTROL_RECORD_BYTE_POINTER.l,a0
                bra.b   .check_sentinel
.refresh_buffered_state:
                bra.w   REFRESH_BUFFERED_COMMAND_STATE
