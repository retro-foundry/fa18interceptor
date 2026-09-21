; Byte-exact indexed-update delta-pair publication $C25E6E-$C25E7D.

                org     $C25E6E

INDEXED_RECORD_CLASS             equ $62

publish_indexed_update_delta_pair:
                move.l  d2,$14(a1)
                move.l  d4,$1C(a1)
                cmpi.b  #$15,INDEXED_RECORD_CLASS(a1)
                dc.w    $6608                   ; bne.b $C25E86
