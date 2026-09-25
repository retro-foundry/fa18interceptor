; Byte-exact observed record-update condition exits $C1425E-$C1429B.
; Each route modifies a frame-local signed word or compares local signed terms,
; then resumes the common update continuation at $C146C2.

                org     $C1425E

RECORD_UPDATE_SHARED_WORD       equ     $C18210
RECORD_UPDATE_CONTINUE          equ     $C146C2
RECORD_UPDATE_DELTA_HELPER      equ     $C14876

handle_record_update_condition_exits:
                subi.w  #$36,-$18(a6)
                bra.w   RECORD_UPDATE_CONTINUE
                moveq   #$DB,d0
                move.l  d0,-(a7)
                bsr.w   RECORD_UPDATE_DELTA_HELPER
                addq.l  #4,a7
                movea.l RECORD_UPDATE_SHARED_WORD.l,a0
                move.w  $26(a0),d0
                add.w   d0,-$18(a6)
                bra.w   RECORD_UPDATE_CONTINUE
                move.w  -$16(a6),d0
                ext.l   d0
                addi.l  #$3C,d0
                move.w  -$1C(a6),d1
                ext.l   d1
                cmp.l   d0,d1
                ble.w   RECORD_UPDATE_CONTINUE
