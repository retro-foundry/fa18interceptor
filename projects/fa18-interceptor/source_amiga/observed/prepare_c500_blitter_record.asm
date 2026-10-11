; Byte-exact observed C500 record preparation $C500D8-$C50125.
; It resolves a two-pointer record chain, writes two DMA-facing words, clamps
; record-derived high words through C501E0, and advances a signed countdown.
; Record ownership remains structural.

                org     $C500D8

DMA_WORD_09C                    equ     $DFF09C
DMA_WORD_096                    equ     $DFF096
CLAMP_RECORD_HIGH_WORDS         equ     $C501E0

prepare_c500_blitter_record:
                movem.l d0-d3/a0-a4,-(sp)
                move.w  $16(a1),DMA_WORD_09C.l
                ; Preserve explicit zero-displacement pointer-load encodings.
                dc.w    $2069,0                 ; movea.l 0(a1),a0
                dc.w    $2469,$0004             ; movea.l 4(a1),a2
                dc.w    $266A,0                 ; movea.l 0(a2),a3
                dc.w    $B7FC,0,0               ; cmpa.l #0,a3
                beq.b   $C50134
                dc.w    $216B,0,0               ; move.l 0(a3),0(a0)
                move.l  4(a3),d0
                asr.l   #1,d0
                move.w  d0,4(a0)
                move.w  $10(a1),DMA_WORD_096.l
                jsr     CLAMP_RECORD_HIGH_WORDS.l
                cmpi.l  #-$1,$10(a3)
                beq.b   $C50152
                subq.l  #1,$10(a3)
                bpl.b   $C50152
