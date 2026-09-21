; Byte-exact generic record-update target pointer path $C23D4A-$C23D71.

                org     $C23D4A

RECORD_ARRAY_BASE                equ $C46184

select_record_update_target_pointer:
                move.w  d0,d1
                andi.w  #$0008,d0
                bne.b   $C23D72
                lea     RECORD_ARRAY_BASE.l,a3
                move.b  $38(a1),d0
                cmpi.b  #$FF,d0
                beq.w   $C2407E
                andi.w  #$007F,d0
                asl.w   #8,d0
                add.w   d0,d0
                adda.w  d0,a3
                bra.w   $C24056
