; Byte-exact strided record mode gate $C22CCE-$C22CDD.
; The non-equal $C22CDE-$C22CE3 path remains outside this slice.

                org     $C22CCE

RECORD_STRIDE_MODE              equ $C458DA

gate_c22cce_record_stride_mode:
                move.w  RECORD_STRIDE_MODE.l,d0
                andi.w  #$F,d0
                cmpi.w  #3,d0
                bne.b   $C22CE4
