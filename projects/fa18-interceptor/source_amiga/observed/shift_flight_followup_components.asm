; Byte-exact lookup/shift phase $C1CDB2-$C1CDFB.
; It consumes D2-D4/D7 from the preceding magnitude phase and falls through to
; the next call at C1CDFC.

                org     $C1CDB2

FOLLOWUP_SHIFT_TABLE            equ     $C1DF46
FOLLOWUP_SHIFT_COUNT             equ     $C45AB8
FOLLOWUP_DEPTH_COMPONENT         equ     $C45A66
FOLLOWUP_SHIFTED_DEPTH           equ     $C45B3C
FOLLOWUP_SHIFTED_COMPONENTS      equ     $C45B30
FOLLOWUP_COMPONENT_WORDS         equ     $C45B2A
FOLLOWUP_SHIFT_READY_FLAG        equ     $C458BB

shift_flight_followup_components:
                lea     FOLLOWUP_SHIFT_TABLE.l,a1
                move.b  (a1,d7.w),d6
                ext.w   d6
                move.w  d6,FOLLOWUP_SHIFT_COUNT.l
                move.l  $18(a0),d3
                move.l  d3,d0
                add.l   FOLLOWUP_DEPTH_COMPONENT.l,d0
                asr.l   d6,d0
                move.l  d0,FOLLOWUP_SHIFTED_DEPTH.l
                move.b  #1,FOLLOWUP_SHIFT_READY_FLAG.l
                asr.l   d6,d2
                asr.l   d6,d3
                asr.l   d6,d4
                movem.l d2-d4,FOLLOWUP_SHIFTED_COMPONENTS.l
                asr.l   #8,d2
                asr.l   #8,d3
                asr.l   #8,d4
                movem.w d2-d4,FOLLOWUP_COMPONENT_WORDS.l
                ; Falls through into the call at C1CDFC.
