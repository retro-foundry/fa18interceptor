; Byte-exact observed paired-word guard $C13B5A-$C13B7D.
; The nonzero-word update path remains raw.

                org     $C13B5A

CURRENT_INDEXED_RECORD_POINTER  equ     $C18210
CLEAR_CURRENT_WORD_5A           equ     $C13B96

guard_current_record_word_6a:
                link.w  a6,#-8
                movea.l CURRENT_INDEXED_RECORD_POINTER.l,a0
                dc.w    $D0FC,$006A ; adda.w #$6a,a0; retain original opcode
                movea.l CURRENT_INDEXED_RECORD_POINTER.l,a1
                dc.w    $D2FC,$005A ; adda.w #$5a,a1; retain original opcode
                move.l  a0,-4(a6)
                move.l  a1,-8(a6)
                tst.w   (a0)
                beq.b   CLEAR_CURRENT_WORD_5A
