; Byte-exact observed C13D84 continuation $C14F6C-$C14F75.
; A nonzero shared long takes the common $C15028 route.

                org     $C14F6C

C13D84_SHARED_LONG             equ     $C461F6

gate_c13d84_shared_long_nonzero:
                tst.l   C13D84_SHARED_LONG.l
                bne.w   $C15028
