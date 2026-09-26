; Byte-exact observed matrix record-word +$6A classifier $C2D5BA-$C2D5CD.
; The word selects a D2 class value via the $3840 and $0118 thresholds, then
; joins the existing class-bit update path.

                org     $C2D5BA

classify_matrix_record_word6a:
                move.w  $6A(a1),d3
                cmpi.w  #$3840,d3
                bgt.b   $C2D5CE
                moveq   #4,d2
                cmpi.w  #$118,d3
                bgt.b   $C2D5DA
                bra.b   $C2D5D8
