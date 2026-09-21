; Byte-exact observed prologue slice $C13D84-$C13E0F.
; It is a function-entry slice, not a claim that the complete $C13D84 body is named.

                org     $C13D84

CURRENT_RECORD_INDEX           equ $C459B4
CONTROL_RECORD_BASE            equ $C46184
CURRENT_RECORD_POINTER          equ $C18210
CONTROL_INPUT_REFERENCE         equ $C4577C

CONTROL_RECORD_STRIDE_SHIFT     equ 9
RECORD_OFFSET_WORD_02           equ 2
RECORD_OFFSET_WORD_04           equ 4
RECORD_OFFSET_BYTE_20           equ $20
RECORD_OFFSET_BYTE_2B           equ $2B
RECORD_OFFSET_BYTE_39           equ $39
RECORD_OFFSET_BYTE_65           equ $65
RECORD_OFFSET_WORD_6C           equ $6C
RECORD_OFFSET_LONG_72           equ $72

NO_ACTIVE_RECORD_ENTRY          equ $C14120
NONZERO_RECORD_INDEX_ENTRY      equ $C13E24

prepare_indexed_control_record_context:
                link.w  a6,#-$32
                movem.l d2/a2-a5,-(a7)
                move.w  CURRENT_RECORD_INDEX.l,d0
                moveq   #CONTROL_RECORD_STRIDE_SHIFT,d1
                move.w  d0,-$32(a6)
                ext.l   d0
                asl.l   d1,d0
                movea.l d0,a0
                adda.l  #CONTROL_RECORD_BASE,a0
                move.l  a0,d0
                move.l  d0,CURRENT_RECORD_POINTER.l
                movea.l d0,a0
                move.l  a0,-$2c(a6)
                addq.l  #RECORD_OFFSET_WORD_02,a0
                movea.l d0,a1
                addq.l  #RECORD_OFFSET_WORD_04,a1
                movea.l d0,a2
                ; Preserve ADDA.W #$20,A2; VASM chooses a shorter equivalent.
                dc.w    $D4FC,RECORD_OFFSET_BYTE_20
                movea.l d0,a3
                dc.w    $D6FC,RECORD_OFFSET_BYTE_2B
                movea.l d0,a4
                dc.w    $D8FC,RECORD_OFFSET_BYTE_65
                movea.l d0,a5
                dc.w    $DAFC,RECORD_OFFSET_WORD_6C
                move.l  a0,-$30(a6)
                movea.l d0,a0
                dc.w    $D0FC,RECORD_OFFSET_BYTE_39
                move.w  (a5),d1
                neg.w   d1
                move.w  d1,-$18(a6)
                move.l  a0,-$c(a6)
                move.l  a1,-$10(a6)
                move.l  a2,-$14(a6)
                move.l  a3,-$4(a6)
                move.l  a4,-$8(a6)
                move.l  a5,-$20(a6)
                movea.l d0,a0
                tst.l   RECORD_OFFSET_LONG_72(a0)
                beq.w   NO_ACTIVE_RECORD_ENTRY
                tst.w   -$32(a6)
                bne.b   NONZERO_RECORD_INDEX_ENTRY
                move.w  CONTROL_INPUT_REFERENCE.l,d0
