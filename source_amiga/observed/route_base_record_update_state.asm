; Byte-exact base-record update state route $C2415E-$C24165.

                org     $C2415E

BASE_RECORD_UPDATE_ENABLE         equ $C4578A

route_base_record_update_state:
                tst.b   BASE_RECORD_UPDATE_ENABLE.l
                bra.b   $C24170
