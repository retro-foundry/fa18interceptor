; Byte-exact observed record-update scaled condition path $C14406-$C14437.
; One entry adjusts a local signed term and rejoins the shared continuation;
; the alternate entry derives a 3/32-scaled local value, then tests two record
; bytes to choose the next route.

                org     $C14406

RECORD_UPDATE_CONTINUE          equ     $C146C2

handle_record_update_scaled_condition:
                addi.w  #$3C,-$18(a6)
                bra.w   RECORD_UPDATE_CONTINUE
                move.w  -$24(a6),d0
                asr.w   #3,d0
                move.w  d0,-$16(a6)
                asr.w   #2,d0
                sub.w   d0,-$16(a6)
                movea.l -$10(a6),a0
                move.b  (a0),d0
                btst    #3,d0
                bne.b   $C1443C
                movea.l -$4(a6),a0
                move.b  (a0),d0
                cmpi.b  #$0C,d0
                bge.b   $C1443C
