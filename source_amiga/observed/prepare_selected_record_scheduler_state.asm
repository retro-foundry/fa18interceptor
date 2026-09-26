; Byte-exact selected-record scheduler setup entries $C0A2F0-$C0A3C5.

                org     $C0A2F0

SCHEDULER_SELECTOR              equ     $C45798
SCHEDULER_EVENT_COUNTER         equ     $C457AE
SCHEDULER_PHASE                 equ     $C4582A
SCHEDULER_COUNTDOWN             equ     $C4582C
SCHEDULER_DELAY_SOURCE          equ     $C458C0
SCHEDULER_MODE_LATCH            equ     $C458AD
SCHEDULER_COMPARE_LEFT          equ     $C458AA
SCHEDULER_COMPARE_RIGHT         equ     $C458AB
PRIMARY_RECORD                  equ     $C46184
SECONDARY_RECORD                equ     $C46984
SCHEDULER_RETURN                equ     $C0A2EE

prepare_primary_record_scheduler_state:
                tst.b   SCHEDULER_SELECTOR.l
                bne.s   SCHEDULER_RETURN
                lea.l   PRIMARY_RECORD.l,a1
                btst.b  #6,1(a1)
                beq.s   SCHEDULER_RETURN
                move.w  2(a1),d0
                andi.w  #$C080,d0
                cmpi.w  #$C080,d0
                bne.s   SCHEDULER_RETURN
                tst.w   $6E(a1)
                bne.s   SCHEDULER_RETURN
                cmpi.b  #3,SCHEDULER_PHASE.l
                beq.s   SCHEDULER_RETURN
                move.b  #$FF,SCHEDULER_SELECTOR.l
                moveq   #0,d0
                bra.w   publish_scheduler_state

primary_return:
                rts

prepare_secondary_record_scheduler_state:
                tst.b   SCHEDULER_SELECTOR.l
                bne.s   primary_return
                lea.l   SECONDARY_RECORD.l,a1
                btst.b  #6,1(a1)
                bne.s   primary_return
                cmpi.b  #3,SCHEDULER_PHASE.l
                beq.s   primary_return
                move.b  #$FE,SCHEDULER_SELECTOR.l
                moveq   #0,d0
                bra.w   publish_scheduler_state

secondary_return:
                rts

prepare_counted_scheduler_state:
                tst.b   SCHEDULER_SELECTOR.l
                bne.w   $C0A3C6
                move.b  SCHEDULER_COMPARE_LEFT.l,d0
                cmp.b   SCHEDULER_COMPARE_RIGHT.l,d0
                bgt.s   secondary_return
                tst.b   SCHEDULER_EVENT_COUNTER.l
                bne.s   decrement_scheduler_event_counter
                move.b  #2,SCHEDULER_EVENT_COUNTER.l
decrement_scheduler_event_counter:
                subq.b  #1,SCHEDULER_EVENT_COUNTER.l
                bne.s   counted_return
                cmpi.b  #3,SCHEDULER_PHASE.l
                beq.s   counted_return
                move.b  #$FF,SCHEDULER_SELECTOR.l
publish_scheduler_state:
                move.w  d0,SCHEDULER_DELAY_SOURCE.l
                move.b  #3,SCHEDULER_PHASE.l
                move.b  #4,SCHEDULER_COUNTDOWN.l
                move.b  #1,SCHEDULER_MODE_LATCH.l
counted_return:
                rts
