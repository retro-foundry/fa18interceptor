; Byte-exact observed C131BE helper return $C1338C-$C13395.
; It sign-extends stack argument +$A into D0, releases the frame, and returns.

                org     $C1338C

return_c131be_signed_stack_argument:
                move.w  $a(a6),d0
                ext.l   d0
                unlk    a6
                rts
