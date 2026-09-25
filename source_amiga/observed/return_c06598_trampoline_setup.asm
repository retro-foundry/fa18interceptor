; Byte-exact observed shared trampoline setup and return $C06598-$C065A9.

                org     $C06598

return_c06598_trampoline_setup:
                move.l  a5,8(sp)
                ; Preserve the captured PC-relative LEA encodings.
                dc.w    $4bfa,$000c
                move.l  a5,4(sp)
                dc.w    $4bfa,$ff7c
                rts
