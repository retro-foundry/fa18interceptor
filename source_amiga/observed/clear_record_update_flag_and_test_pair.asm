; Byte-exact observed C13D84 flag-clear and local-pair test $C14708-$C1472B.
; It clears bit 14 in a shared word, resets local -$16, then tests whether the
; top two bits of the local record word are both set.

                org     $C14708

RECORD_UPDATE_FLAGS             equ     $C458D2

clear_record_update_flag_and_test_pair:
                move.w  RECORD_UPDATE_FLAGS.l,d0
                andi.w  #$BFFF,d0
                move.w  d0,RECORD_UPDATE_FLAGS.l
                clr.w   -$16(a6)
                movea.l -$30(a6),a0
                move.w  (a0),d0
                andi.w  #$C000,d0
                cmpi.w  #$C000,d0
                bne.b   $C14744
