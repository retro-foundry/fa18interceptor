; Byte-exact common command-input queue $C1C23C-$C1C2B7.
; Reached by multiple sealed run002/run003 raw-key command handlers.

                org     $C1C23C

COMMAND_INPUT_PENDING           equ $C457A3
COMMAND_QUEUE_COUNT              equ $C457F9
COMMAND_QUEUE_CAPACITY           equ 10
COMMAND_QUEUE_WRITE_INDEX        equ $C457F7
COMMAND_QUEUE_TRANSLATED_INDEX   equ $C457F6
COMMAND_QUEUE_RAW_EVENTS         equ $C457E1
COMMAND_QUEUE_TRANSLATED_EVENTS  equ $C457EB
COMMAND_KEY_TRANSLATION_TABLE    equ $C331CE
INPUT_TRANSIENT_A                equ $C45878
INPUT_TRANSIENT_B                equ $C45879
INPUT_TRANSIENT_C                equ $C4587A

enqueue_command_input_event:
                tst.b   COMMAND_INPUT_PENDING.l
                bne.s   clear_input_transients
                btst    #7,d0
                bne.s   clear_input_transients
                move.b  #1,COMMAND_INPUT_PENDING.l
                cmpi.b  #COMMAND_QUEUE_CAPACITY,COMMAND_QUEUE_COUNT.l
                bge.s   clear_input_transients
                move.b  COMMAND_QUEUE_WRITE_INDEX.l,d4
                cmpi.b  #COMMAND_QUEUE_CAPACITY,d4
                blt.s   .store_raw_event
                moveq   #0,d4
.store_raw_event:
                ext.w   d4
                lea.l   COMMAND_QUEUE_RAW_EVENTS.l,a3
                move.b  d0,(a3,d4.w)
                andi.w  #$FF,d0
                lea.l   COMMAND_KEY_TRANSLATION_TABLE.l,a3
                move.b  (a3,d0.w),d0
                addq.b  #1,d4
                move.b  d4,COMMAND_QUEUE_WRITE_INDEX.l
                addq.b  #1,COMMAND_QUEUE_COUNT.l
                lea.l   COMMAND_QUEUE_TRANSLATED_EVENTS.l,a3
                move.b  COMMAND_QUEUE_TRANSLATED_INDEX.l,d4
                ext.w   d4
                move.b  d0,(a3,d4.w)
clear_input_transients:
                clr.b   INPUT_TRANSIENT_A.l
                clr.b   INPUT_TRANSIENT_B.l
                clr.b   INPUT_TRANSIENT_C.l
                rts
