; Byte-exact observed record-local byte update gate $C12CF0-$C12D29.
; A frame-local byte and low bits of a tick counter select an early exit.
; Otherwise the active record class and signed byte at +$2B control the next
; local update route.

                org     $C12CF0

POST_INPUT_TICK_COUNTER         equ     $C457C1
ACTIVE_CONTROL_RECORD           equ     $C18210

gate_record_local_byte_update:
                tst.b   -$11(a6)
                bne.b   $C12D06
                move.b  POST_INPUT_TICK_COUNTER.l,d0
                andi.b  #$03,d0
                tst.b   d0
                bne.w   $C1313A
                movea.l ACTIVE_CONTROL_RECORD.l,a0
                move.b  $62(a0),d0
                andi.b  #$F0,d0
                cmpi.b  #$30,d0
                beq.w   $C13124
                move.b  $2B(a0),d0
                ext.w   d0
                move.w  d0,-$2(a6)
                tst.w   d0
                bpl.b   $C12D2E
