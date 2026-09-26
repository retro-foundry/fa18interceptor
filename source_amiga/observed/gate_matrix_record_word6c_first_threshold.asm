; Byte-exact observed matrix-record continuation $C2D594-$C2D5A1.
; It sets record word-0 bit $2000, then compares word +$6C with the first
; observed threshold before selecting the next adjustment route.

                org     $C2D594

gate_matrix_record_word6c_first_threshold:
                dc.w    $0069,$2000,$0000        ; ori.w #$2000,$0(a1)
                cmpi.w  #$1000,$6c(a1)
                bge.b   $C2D5A8
