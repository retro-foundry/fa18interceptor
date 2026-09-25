; Byte-exact observed basic block $C15200-$C15217.

                org     $C15200

RECORD_SCAN_CURRENT_SLOT        equ     $C18214
RECORD_SCAN_MODE_BYTE           equ     $C457AE
RECORD_SCAN_MODE_FLAG_PATH      equ     $C15224

prepare_c15200_record_scan_slot:
                movea.l RECORD_SCAN_CURRENT_SLOT.l,a0
                ; ADDA.W #$0026,A0; retained as words: assembler selects an
                ; alternate address-register immediate encoding.
                dc.w    $d0fc,$0026
                move.b  RECORD_SCAN_MODE_BYTE.l,d0
                move.l  a0,-$14(a6)
                tst.b   d0
                bne.b   RECORD_SCAN_MODE_FLAG_PATH
