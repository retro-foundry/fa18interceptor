; Byte-exact indexed-update entry gate $C25B66-$C25B6F.
; The bounded frame-6000 route takes the zero-index BEQ.

                org     $C25B66

SELECTED_RECORD_INDEX            equ $C459B4

initialize_indexed_update_gate:
                moveq   #0,d0
                tst.w   SELECTED_RECORD_INDEX.l
                dc.w    $6708                   ; beq.b $C25B78
