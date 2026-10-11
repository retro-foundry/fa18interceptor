; Byte-exact current-record class dispatch $C2D408-$C2D419.

                org     $C2D408

CONTINUE_NONCLASS30_RECORD      equ     $C2D496

dispatch_record_class_30:
                move.b  $62(a1),d0
                andi.b  #$f0,d0
                cmpi.b  #$30,d0
                bne.w   CONTINUE_NONCLASS30_RECORD
