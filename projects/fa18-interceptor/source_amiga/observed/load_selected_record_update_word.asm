; Byte-exact selected-record update-word gate $C23BB0-$C23BBD.

                org     $C23BB0

load_selected_record_update_word:
                move.w  $6C(a2),d1
                move.w  d1,d0
                dc.w    $0829,$0005,$0004       ; btst.b #5,$04(a1)
                dc.w    $6714                   ; beq.b $C23BD2
