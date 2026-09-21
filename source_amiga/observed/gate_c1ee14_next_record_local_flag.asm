; Byte-exact next-record local flag gate $C1F26C-$C1F273.
; The taken path starts at the separately unobserved $C1F274.

                org     $C1F26C

gate_c1ee14_next_record_local_flag:
                btst.b  #0,-127(a6)
                beq.b   $C1F27A
