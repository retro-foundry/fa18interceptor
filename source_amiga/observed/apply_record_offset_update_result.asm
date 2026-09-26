; Byte-exact observed record-offset update result route $C144D8-$C144FF.
; It passes local word -$22 through the C14876 helper, then either rejoins the
; shared update exit or adds active-record word +$26 to local -$18.

                org     $C144D8

RECORD_OFFSET_UPDATE_HELPER     equ     $C14876
CURRENT_CONTROL_RECORD          equ     $C18210
RECORD_UPDATE_CONTINUE          equ     $C146C2

apply_record_offset_update_result:
                move.w  -$22(a6),d0
                ext.l   d0
                move.l  d0,-(a7)
                bsr.w   RECORD_OFFSET_UPDATE_HELPER
                addq.l  #4,a7
                tst.w   -$22(a6)
                beq.w   RECORD_UPDATE_CONTINUE
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.w  $26(a0),d0
                add.w   d0,-$18(a6)
                bra.w   RECORD_UPDATE_CONTINUE
