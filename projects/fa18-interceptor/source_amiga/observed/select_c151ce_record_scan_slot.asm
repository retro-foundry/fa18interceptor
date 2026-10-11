; Byte-exact observed loop block $C151CE-$C151E7.

                org     $C151CE

RECORD_SCAN_TABLE_BASE          equ     $C45C72
RECORD_SCAN_CURRENT_SLOT        equ     $C18214
RECORD_SCAN_DECREMENT_TEST      equ     $C151E8
RECORD_SCAN_SLOT_SETUP          equ     $C15200

select_c151ce_record_scan_slot:
                ext.l   d0
                asl.l   #6,d0
                movea.l d0,a0
                adda.l  #RECORD_SCAN_TABLE_BASE,a0
                move.l  a0,RECORD_SCAN_CURRENT_SLOT.l
                tst.b   $C457BD.l
                bne.b   RECORD_SCAN_SLOT_SETUP
