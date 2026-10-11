; Byte-exact observed indexed-record scaled triple load $C25DAE-$C25DC9.
; It reads three signed words at +$96/+ $9C/+ $A2 and divides each by four
; with arithmetic shifts before the adjacent record-flag gate.

                org     $C25DAE

load_scaled_indexed_record_triple:
                move.w  $96(a1),d5
                move.w  $9C(a1),d6
                move.w  $A2(a1),d7
                asr.w   #2,d5
                asr.w   #2,d6
                asr.w   #2,d7
                dc.w    $3029,$0000             ; move.w $0(a1),d0
                andi.w  #$100,d0
                beq.b   $C25DDA
