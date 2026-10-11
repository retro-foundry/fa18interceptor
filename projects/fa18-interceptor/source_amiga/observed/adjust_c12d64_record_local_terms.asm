; Byte-exact observed record-local adjustment $C12D64-$C12DA9.
; It derives an absolute shifted word from current-record +$56, conditionally
; accumulates it into a local, and bounds a second local subtraction.

                org     $C12D64

CURRENT_CONTROL_RECORD          equ     $C18210
RECORD_OFFSET_WORD_56           equ     $56

adjust_c12d64_record_local_terms:
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.w  RECORD_OFFSET_WORD_56(a0),d0
                asr.w   #4,d0
                move.w  d0,-$a(a6)
                tst.w   d0
                bpl.b   $C12D7C
                neg.w   -$a(a6)
                move.w  -$c(a6),d0
                cmp.w   -$a(a6),d0
                bge.b   $C12D96
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.w  RECORD_OFFSET_WORD_56(a0),d1
                asr.w   #4,d1
                add.w   d1,-$c(a6)
                move.w  -$2(a6),d0
                cmpi.w  #$78,d0
                blt.b   $C12DAA
                move.w  -$c(a6),d0
                sub.w   d0,-$2(a6)
                bra.b   $C12DC4
