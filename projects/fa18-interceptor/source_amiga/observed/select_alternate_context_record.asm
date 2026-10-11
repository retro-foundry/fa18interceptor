; Byte-exact alternate-context record selector $C1BE60-$C1BEE7.

                org     $C1BE60

COMMAND_CONTEXT_FLAGS           equ $C458CC
COMMAND_CONTEXT_FLAG_BIT        equ 3
SIGNED_RECORD_TABLE             equ $C42A02
COMMAND_RECORD_INDEX            equ $C45848
COMMAND_CONTEXT_STATE           equ $C458AE
COMMAND_CONTEXT_GATE            equ $C458AD
COMMAND_STATE_POINTER           equ $C457B5
COMMAND_ENTRY_STATE             equ $C457A3
COMMAND_ENTRY_STATE_DISABLED    equ $FF
COMMAND_CONTEXT_FLAG            equ $C45785
COMMAND_MODE                    equ $C4584B
COMMAND_MODE_TWO                equ 2

PREPARE_COMMAND_QUEUE_CONTEXT   equ $C1C214
CONTEXT_SPECIAL_REQUEST_RETRY   equ $C1B684
SHARED_COMMAND_QUEUE            equ $C1C23C

select_alternate_context_record:
                btst.b  #COMMAND_CONTEXT_FLAG_BIT,COMMAND_CONTEXT_FLAGS.l
                bne.b   select_mode_adjustment
                lea.l   SIGNED_RECORD_TABLE.l,a0
                move.b  d4,d1
                ext.w   d1
                asl.w   #4,d1
                clr.w   d2
.scan_record_table:
                move.w  (a0,d2.w),d3
                blt.b   .skip_negative_record
                cmp.w   d2,d1
                ble.b   .store_record_index
                addi.w  #$10,d2
                bra.b   .scan_record_table
.skip_negative_record:
                addq.w  #1,d3
                beq.w   SHARED_COMMAND_QUEUE
.store_record_index:
                move.b  d4,COMMAND_RECORD_INDEX.l
                tst.b   COMMAND_CONTEXT_STATE.l
                blt.w   SHARED_COMMAND_QUEUE
                tst.b   COMMAND_CONTEXT_GATE.l
                bne.w   SHARED_COMMAND_QUEUE
                lea.l   COMMAND_STATE_POINTER.l,a0
                bra.w   PREPARE_COMMAND_QUEUE_CONTEXT

retry_context_special_request:
                move.b  #COMMAND_ENTRY_STATE_DISABLED,COMMAND_ENTRY_STATE.l
                tst.b   COMMAND_CONTEXT_FLAG.l
                bne.w   CONTEXT_SPECIAL_REQUEST_RETRY
                bra.w   SHARED_COMMAND_QUEUE

select_mode_adjustment:
                move.b  COMMAND_MODE.l,d2
                beq.w   .increment_mode
                cmpi.b  #COMMAND_MODE_TWO,d2
                beq.w   .increment_mode
                bra.w   SHARED_COMMAND_QUEUE
.increment_mode:
                addq.w  #1,d4
                bra.b   .queue_adjusted_mode
.clear_mode:
                clr.w   d4
.queue_adjusted_mode:
                bra.w   SHARED_COMMAND_QUEUE
