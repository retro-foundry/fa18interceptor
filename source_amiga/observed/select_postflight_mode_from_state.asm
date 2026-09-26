; Byte-exact observed postflight mode-selection gate $C11016-$C11035.
; A zero word and byte pair, followed by a byte greater than five, select the
; value 2 path.  The adjacent shared path selects value 15 instead.

                org     $C11016

POSTFLIGHT_MODE_BYTE            equ     $C458A1

select_postflight_mode_from_state:
                move.w  $C458DE,d0
                tst.w   d0
                bne.b   $C11040
                move.b  $C45785,d0
                tst.b   d0
                bne.b   $C11040
                move.b  $C45889,d0
                cmpi.b  #5,d0
                ble.b   $C11040
                move.b  #2,POSTFLIGHT_MODE_BYTE
