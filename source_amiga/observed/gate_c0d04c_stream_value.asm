; Byte-exact observed stream-value gate $C0D04C-$C0D059.

                org     $C0D04C

C0D048                         equ     $C0D048
C459B6                         equ     $C459B6
C4FDD2                         equ     $C4FDD2

gate_c0d04c_stream_value:
                move.w  C459B6.l,d0
                cmp.w   C4FDD2.l,d0
                bne.b   C0D048
