; Byte-exact observed basic block $C151E8-$C151FF.

                org     $C151E8

RECORD_SCAN_MODE_BYTE           equ     $C457AE
RECORD_SCAN_CURRENT_SLOT        equ     $C18214
RECORD_SCAN_SLOT_SETUP          equ     $C15200

decrement_c151e8_record_scan_slot:
                tst.b   RECORD_SCAN_MODE_BYTE.l
                bne.b   RECORD_SCAN_SLOT_SETUP
                move.w  $28(a0),d0
                subq.w  #1,d0
                movea.l RECORD_SCAN_CURRENT_SLOT.l,a0
                move.w  d0,$28(a0)
