; Byte-exact observed matrix-side continuation $C1396A-$C13977.
; It sign-extends local -$2, supplies it as the sole long argument to the
; C13B5A helper, and removes that argument after return.

                org     $C1396A

call_c13b5a_with_signed_local:
                move.w  -$2(a6),d0
                ext.l   d0
                move.l  d0,-(sp)
                bsr.w   $C13B5A
                addq.l  #4,sp
