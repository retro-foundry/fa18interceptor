; Byte-exact record-update helper state advance $C24568-$C24575.

                org     $C24568

advance_record_update_helper_state:
                addi.b  #$10,$39(a1)
                cmpi.w  #$0480,$4A(a1)
                blt.b   $C2458E
