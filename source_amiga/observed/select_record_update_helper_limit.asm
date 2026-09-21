; Byte-exact record-update helper limit selection $C24576-$C24581.

                org     $C24576

select_record_update_helper_limit:
                moveq   #$20,d1
                cmpi.w  #$0900,$4A(a1)
                blt.b   $C24582
                moveq   #$50,d1
