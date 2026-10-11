; Byte-exact observed signed control-word-delta helper prefix $C13CDE-$C13CFB.
; It derives a signed local delta from current-record word +$6C, loads the
; caller-supplied target word, and routes negative values to the paired path.

                org     $C13CDE

CURRENT_CONTROL_RECORD          equ     $C18210

prepare_signed_control_word_delta:
                link.w  a6,#-2
                dc.w    $2079                   ; movea.l $C18210,a0
                dc.l    CURRENT_CONTROL_RECORD
                move.w  $6C(a0),d0
                asr.w   #7,d0
                movea.l 8(a6),a0
                move.w  (a0),d1
                move.w  d0,-2(a6)
                tst.w   d1
                bmi.b   $C13D14
