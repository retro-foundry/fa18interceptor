; Byte-exact observed candidate-table component gate $C2715C-$C2717B.
; It selects the next table pointer, uses the candidate byte's low nibble as a
; signed component shift, and enters the candidate scan only when D3 meets
; the shifted table-component bound.

                org     $C2715C

gate_candidate_table_component:
                movea.l (a4)+,a5
                move.w  (a5),d7
                addi.w  #$A4,d7
                move.b  $7D(a3),d2
                andi.w  #$000F,d2
                move.w  d2,-$5A(a6)
                move.w  $2(a3,d7.w),d4
                asr.w   d2,d4
                cmp.w   d4,d3
                bge.w   $C27198
