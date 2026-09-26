; Byte-exact observed indexed-update continuation $C25BAC-$C25BB7.
; It retains record word zero in D1 and tests its $0400 bit before the shared
; command-bit routing path.

                org     $C25BAC

gate_indexed_update_record_bit10:
                dc.w    $3029,$0000             ; move.w $0(a1),d0
                move.w  d0,d1
                andi.w  #$400,d0
                beq.b   $C25C2E
