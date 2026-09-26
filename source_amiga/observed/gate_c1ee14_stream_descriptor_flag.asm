; Byte-exact observed C1EE14 stream-descriptor continuation $C1EEE4-$C1EEED.
; It initializes D7 to one and gates the following descriptor route on a
; shared byte flag.

                org     $C1EEE4

STREAM_DESCRIPTOR_FLAG          equ     $C4579E

gate_c1ee14_stream_descriptor_flag:
                moveq   #1,d7
                tst.b   STREAM_DESCRIPTOR_FLAG.l
                beq.b   $C1EEFA
