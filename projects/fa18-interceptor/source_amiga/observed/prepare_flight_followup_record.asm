; Byte-exact setup prefix $C1CCBC-$C1CD0D of the parent-called follow-up stage.
; It initializes mutable placement fields and resolves an indexed C46184 record.

                org     $C1CCBC

FOLLOWUP_AUX_FLAG               equ     $C45864
FOLLOWUP_SELECTOR_BYTE          equ     $C4585B
FOLLOWUP_RECORD_BYTE             equ     $C458BD
FOLLOWUP_RECORD_VALUE            equ     $C45ABA
FOLLOWUP_WORK_LONG               equ     $C45932
FOLLOWUP_RECORD_INDEX            equ     $C459AA
FOLLOWUP_RECORD_INDEXED_WORD     equ     $C459B4
FOLLOWUP_RECORD_SCALED_WORD      equ     $C459B6
FOLLOWUP_RECORD_TABLE            equ     $C4E98A
FOLLOWUP_RECORD_BASE             equ     $C46184
FOLLOWUP_EMPTY_CONTINUATION      equ     $C1CE38

prepare_flight_followup_record:
                move.b  #1,FOLLOWUP_AUX_FLAG.l
                move.b  #4,FOLLOWUP_SELECTOR_BYTE.l
                clr.b   FOLLOWUP_RECORD_BYTE.l
                clr.w   FOLLOWUP_RECORD_VALUE.l
                clr.l   FOLLOWUP_WORK_LONG.l
                clr.w   FOLLOWUP_RECORD_INDEX.l
                lea     FOLLOWUP_RECORD_TABLE.l,a0
                dc.w    $D0F9,$00C4,$59AA     ; adda.w FOLLOWUP_RECORD_INDEX.l,a0
                move.w  (a0),d0
                ble.w   FOLLOWUP_EMPTY_CONTINUATION
                lea     FOLLOWUP_RECORD_BASE.l,a0
                move.w  d0,FOLLOWUP_RECORD_INDEXED_WORD.l
                asl.w   #8,d0
                add.w   d0,d0
                move.w  d0,FOLLOWUP_RECORD_SCALED_WORD.l
                adda.w  d0,a0
                ; Falls through into the component-calculation phase at C1CD0E.
