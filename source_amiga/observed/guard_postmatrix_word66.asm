; Byte-exact observed word-$66 bound guard $C2D800-$C2D80B.
; The below-bound body starts at raw $C2D80C.

                org     $C2D800

                move.w  $66(a1),d0
                asr.w   #1,d0
                cmpi.w  #$1c20,d0
                bge.b   $C2D81A
