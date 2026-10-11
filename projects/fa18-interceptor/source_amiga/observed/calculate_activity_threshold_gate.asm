; Byte-exact observed activity-calculation continuation $C25416-$C2547D.
; The arithmetic publishes a bounded accumulator and compares its quotient to
; an indexed local table; user-facing semantics are not yet proven.

                org     $C25416

ACTIVITY_SOURCE_A                equ     $C45AF2
ACTIVITY_SOURCE_B                equ     $C45AF6
ACTIVITY_REFERENCE_B             equ     $C45AFE
ACTIVITY_REFERENCE_COPY          equ     $C45B06
ACTIVITY_THRESHOLD_ACCUMULATOR   equ     $C45B0A
FRAME_COUNTER_WORD               equ     $C458DA
ACTIVITY_TABLE_INDEX             equ     $C458BE
ACTIVITY_PREPARE_HELPER          equ     $C16D04

calculate_activity_threshold_gate:
                move.l  ACTIVITY_SOURCE_B.l,ACTIVITY_REFERENCE_COPY.l
                jsr     ACTIVITY_PREPARE_HELPER.l
                move.l  $C45AFA.l,d1
                blt.b   $C25480
                sub.l   ACTIVITY_SOURCE_A.l,d1
                neg.l   d1
                mulu.w  #$3E8,d1
                move.l  ACTIVITY_SOURCE_B.l,d0
                sub.l   ACTIVITY_REFERENCE_B.l,d0
                divs.w  #$3E8,d0
                ext.l   d0
                add.l   d0,d1
                move.l  d1,ACTIVITY_THRESHOLD_ACCUMULATOR.l
                cmpi.l  #$7FFF,d1
                bgt.b   $C25480
                move.w  FRAME_COUNTER_WORD.l,d2
                andi.w  #7,d2
                addq.w  #1,d2
                divu.w  d2,d1
                lea     $C2502E(pc),a0
                move.b  ACTIVITY_TABLE_INDEX.l,d2
                ext.w   d2
                add.w   d2,d2
                cmp.w   (a0,d2.w),d1
                bge.b   $C25480
