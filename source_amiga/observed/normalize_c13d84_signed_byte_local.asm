; Byte-exact observed C13D84 signed-byte local normalization $C14008-$C1401D.
; With the local gate clear, it sign-extends the referenced byte into a word
; local and clamps that local to zero when negative.

                org     $C14008

normalize_c13d84_signed_byte_local:
                tst.w   -$32(a6)
                bne.b   $C1406C
                movea.l -$4(a6),a0
                move.b  (a0),d0
                ext.w   d0
                move.w  d0,-$24(a6)
                tst.w   d0
                bpl.b   $C14022
