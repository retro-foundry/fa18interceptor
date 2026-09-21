; Byte-exact selected-record pointer setup $C23B82-$C23BA3.

                org     $C23B82

RECORD_ARRAY_BASE                equ $C46184

initialize_record_update_selected_pointer:
                lea     RECORD_ARRAY_BASE.l,a2
                move.b  $38(a1),d1
                cmpi.b  #$FF,d1
                beq.b   $C23BB0
                andi.w  #$007F,d1
                asl.w   #8,d1
                add.w   d1,d1
                adda.w  d1,a2
                dc.w    $0829,$0006,$0001       ; btst.b #6,$01(a1)
                bne.b   $C23BB0
