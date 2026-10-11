; Byte-exact zero-result candidate class gate $C262B4-$C262C1.

                org     $C262B4

INDEXED_RECORD_CLASS             equ $62

gate_zero_candidate_scan_class:
                move.b  INDEXED_RECORD_CLASS(a1),d2
                andi.b  #$F0,d2
                cmpi.b  #$30,d2
                dc.w    $67BC                   ; beq.b $C2627E
