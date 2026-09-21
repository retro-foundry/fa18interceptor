; Byte-exact indexed-record selector call $C25D5E-$C25D85.

                org     $C25D5E

INDEXED_RECORD_CLASS             equ $62
RUN_INDEXED_RECORD_SELECTOR      equ $C13D84

run_indexed_record_selector:
                btst.b  #1,$20(a1)
                bne.b   $C25D86
                move.b  INDEXED_RECORD_CLASS(a1),d0
                andi.b  #$F0,d0
                cmpi.b  #$10,d0
                bne.b   $C25D86
                dc.w    $0829,$0004,$0000       ; btst.b #4,$00(a1)
                dc.w    $670A                   ; beq.b $C25D86
                move.l  a1,-(a7)
                jsr     RUN_INDEXED_RECORD_SELECTOR.l
                movea.l (a7)+,a1
