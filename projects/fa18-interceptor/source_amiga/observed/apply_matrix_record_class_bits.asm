; Byte-exact observed matrix-record class-bit update $C2D5D8-$C2D5F5.
; It merges D2's class bits into record byte +$65, then raises bit 4 of byte
; +$03 when bits 1-2 of byte +$64 are both set.

                org     $C2D5D8

apply_matrix_record_class_bits:
                moveq   #0,d2
                move.b  $65(a1),d1
                andi.b  #3,d1
                or.b    d2,d1
                move.b  d1,$65(a1)
                move.b  $64(a1),d1
                andi.b  #6,d1
                cmpi.b  #6,d1
                bne.b   $C2D5FC
                bset    #4,$3(a1)
