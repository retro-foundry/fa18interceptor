; Byte-exact observed record-transform group scan $C1E640-$C1E673.
; It publishes the next group index and sentinel count, advances through
; $18-byte records, and routes according to the current cursor relation and
; a record bit before the existing transform continuations.

                org     $C1E640

TRANSFORM_GROUP_INDEX           equ     $C4FD5C
TRANSFORM_GROUP_COUNT           equ     $C4FD5E

advance_record_transform_group_scan:
                addq.w  #1,d5
                move.w  d5,TRANSFORM_GROUP_INDEX.l
                move.w  #-$1,TRANSFORM_GROUP_COUNT.l
                bra.b   $C1E656
                ; Preserve the binary's immediate ADDA encoding.
                dc.w    $D2FC,$0018             ; adda.w #$18,a1
                tst.w   (a1)
                blt.w   $C1EAEC
                move.l  a1,d1
                cmp.l   a0,d1
                beq.w   $C1EADE
                blt.b   $C1E6D6
                move.w  (a0),d1
                btst    #4,d1
                beq.b   $C1E674
                bsr.w   $C1EBC0
                bra.b   $C1E67C
