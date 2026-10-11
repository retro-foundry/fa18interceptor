; Byte-exact observed guard prefix $C13A8E-$C13AB5.
; The positive-word continuation remains raw.

                org     $C13A8E

CURRENT_INDEXED_RECORD_POINTER  equ     $C18210
RETURN_CURRENT_WORD_66          equ     $C13B56

guard_current_record_word_66:
                link.w  a6,#-$e
                movea.l CURRENT_INDEXED_RECORD_POINTER.l,a0
                dc.w    $D0FC,$0066 ; adda.w #$66,a0; retain original opcode
                movea.l CURRENT_INDEXED_RECORD_POINTER.l,a1
                dc.w    $D2FC,$0056 ; adda.w #$56,a1; retain original opcode
                move.w  (a0),d0
                move.l  a0,-6(a6)
                move.l  a1,-10(a6)
                tst.w   d0
                ble.w   RETURN_CURRENT_WORD_66
