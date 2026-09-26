; Byte-exact observed record-stream continuation $C1ED70-$C1ED7B.
; It compares a shared byte against the observed value three and takes the
; low-value branch when it is smaller.

                org     $C1ED70

RECORD_STREAM_STATE_BYTE        equ     $C457A7

gate_c1ed4c_shared_byte_lower_bound:
                move.b  RECORD_STREAM_STATE_BYTE.l,d0
                cmpi.b  #3,d0
                blt.b   $C1ED84
