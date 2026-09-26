; Byte-exact observed matrix-record continuation $C2D5A8-$C2D5B5.
; It applies the second observed word +$6C threshold and selects the matching
; fixed D4 adjustment before the shared continuation.

                org     $C2D5A8

gate_matrix_record_word6c_second_threshold:
                cmpi.w  #$2000,$6c(a1)
                bge.b   $C2D5B6
                move.w  #$fce0,d4
                bra.b   $C2D5BA
