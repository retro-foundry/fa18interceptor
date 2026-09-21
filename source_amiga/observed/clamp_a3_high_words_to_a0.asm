; Byte-exact structural leaf $C501E0-$C50211.
; A0/A3 record ownership and all field meaning remain unassigned.
                org     $C501E0

A3_FIRST_LONG_OFFSET            equ $08
A3_SECOND_LONG_OFFSET           equ $0C
A0_FIRST_WORD_OFFSET            equ $06
A0_SECOND_WORD_OFFSET           equ $08
FIRST_HIGH_WORD_MINIMUM         equ $7C
SECOND_HIGH_WORD_MASK           equ $3F
SECOND_HIGH_WORD_MAXIMUM        equ $C4FF26

clamp_a3_high_words_to_a0:
                move.l  A3_FIRST_LONG_OFFSET(a3),d0
                swap    d0
                cmpi.w  #FIRST_HIGH_WORD_MINIMUM,d0
                bge.s   .store_first_word
                ; Preserve captured MOVE.W #$007C,D0 rather than MOVEQ.
                dc.w    $303C,FIRST_HIGH_WORD_MINIMUM
.store_first_word:
                move.w  d0,A0_FIRST_WORD_OFFSET(a0)
                move.l  A3_SECOND_LONG_OFFSET(a3),d0
                swap    d0
                andi.w  #SECOND_HIGH_WORD_MASK,d0
                cmp.w   SECOND_HIGH_WORD_MAXIMUM.l,d0
                ble.s   .store_second_word
                move.w  SECOND_HIGH_WORD_MAXIMUM.l,d0
.store_second_word:
                move.w  d0,A0_SECOND_WORD_OFFSET(a0)
                rts
