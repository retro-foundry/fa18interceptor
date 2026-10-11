; Byte-exact observed activity-accumulator block $C25346-$C253B1.
; The entry's preceding conditional body is outside this captured slice.

                org     $C25346

ACTIVITY_SOURCE_A                equ     $C45AF2
ACTIVITY_SOURCE_B                equ     $C45AF6
ACTIVITY_REFERENCE_A             equ     $C45B02
ACTIVITY_REFERENCE_B             equ     $C45B06
ACTIVITY_LONG_ACCUMULATOR        equ     $C45B10
ACTIVITY_LONG_REFERENCE          equ     $C45B14
ACTIVITY_WORD_ACCUMULATOR        equ     $C45B0E
ACTIVITY_MODE_WORD               equ     $C458CE
ACTIVITY_UPDATE_TARGET           equ     $C25416

update_c25346_activity_accumulators:
                sub.l   ACTIVITY_SOURCE_A.l,d1
                neg.l   d1
                mulu.w  #$3E8,d1
                move.l  ACTIVITY_SOURCE_B.l,d0
                sub.l   ACTIVITY_REFERENCE_B.l,d0
                divs.w  #$3E8,d0
                ext.l   d0
                add.l   d0,d1
                add.l   d1,ACTIVITY_LONG_ACCUMULATOR.l
                add.w   d1,ACTIVITY_WORD_ACCUMULATOR.l
                cmpi.w  #$7D,ACTIVITY_WORD_ACCUMULATOR.l
                blt.b   .word_ready
                clr.w   ACTIVITY_WORD_ACCUMULATOR.l
.word_ready:
                move.l  ACTIVITY_LONG_ACCUMULATOR.l,d1
                sub.l   ACTIVITY_LONG_REFERENCE.l,d1
                cmpi.l  #$2710,d1
                blt.b   .done
                ori.w   #$40,ACTIVITY_MODE_WORD.l
                add.l   d1,ACTIVITY_LONG_REFERENCE.l
.done:
                move.l  ACTIVITY_SOURCE_A.l,d0
                cmp.l   ACTIVITY_REFERENCE_A.l,d0
                beq.b   ACTIVITY_UPDATE_TARGET
