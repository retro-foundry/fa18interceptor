; Byte-exact zero-word continuation $C13B96-$C13B9F.

                org     $C13B96

clear_current_record_word_5a:
                movea.l -8(a6),a0
                clr.w   (a0)
                unlk    a6
                rts
