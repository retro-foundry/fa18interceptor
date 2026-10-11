; Byte-exact stream-record sentinel and flag gates $C1EE84-$C1EE91.

                org     $C1EE84

gate_c1ee14_stream_record:
                cmpi.w  #-$1,d0
                beq.b   $C1EEA0
                move.w  d0,d1
                andi.w  #$2000,d1
                beq.b   $C1EEA6
