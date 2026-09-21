; Byte-exact base-record update state gate $C24128-$C24133.

                org     $C24128

BASE_RECORD_UPDATE_STATE          equ $C458DA

gate_base_record_update_state:
                move.w  BASE_RECORD_UPDATE_STATE.l,d3
                andi.w  #$00FF,d3
                bne.b   $C2415E
