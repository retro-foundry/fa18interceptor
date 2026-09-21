; Byte-exact indexed-record matrix-dispatch class gate $C25D86-$C25D93.

                org     $C25D86

INDEXED_RECORD_CLASS             equ $62

gate_indexed_record_matrix_dispatch:
                move.b  INDEXED_RECORD_CLASS(a1),d0
                andi.b  #$F0,d0
                cmpi.b  #$30,d0
                dc.w    $6708                   ; beq.b $C25D9C
