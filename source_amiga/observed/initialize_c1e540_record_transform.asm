; Byte-exact observed C1E540 record-transform entry $C1E540-$C1E579.
; It scans $18-byte slots until a negative lead word, backs up to that slot,
; and loads its record pointer before handing off to the existing continuation.

                org     $C1E540

RECORD_TRANSFORM_SLOT_LIST      equ     $C4F6CA

initialize_c1e540_record_transform:
                link.w  a6,#-$22
                lea     RECORD_TRANSFORM_SLOT_LIST.l,a1
                moveq   #-$1,d5
scan_record_transform_slot:
                tst.w   (a1)
                blt.b   $C1E558
                addq.w  #1,d5
                ; Preserve the binary's immediate ADDA encoding.
                dc.w    $D2FC,$0018             ; adda.w #$18,a1
                bra.b   scan_record_transform_slot
                cmpi.w  #1,d5
                blt.b   $C1E53C
                ; Preserve the binary's immediate SUBA encoding.
                dc.w    $92FC,$0018             ; suba.w #$18,a1
                move.w  (a1),d1
                movea.l $2(a1),a2
                move.l  $4(a2),d7
                ble.b   $C1E598
                movea.l d7,a0
                move.w  (a0),d7
                blt.b   $C1E584
                andi.w  #$4000,d7
                beq.b   $C1E580
