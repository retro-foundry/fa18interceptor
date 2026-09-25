; Byte-exact observed indexed record-group copy setup $C1EAEC-$C1EB3D.
; It copies counted six-longword groups into the mutable C48390 workspace, then
; begins a header-classified follow-up pass.  Group identity remains structural.

                org     $C1EAEC

COPY_GROUP_COUNT                equ     $C4FD5E
COPY_GROUP_INDEX                equ     $C4FD5C
COPY_GROUP_SOURCE               equ     $C4F6CA
TRANSFORM_WORKSPACE             equ     $C48390

copy_indexed_transformed_record_groups:
                move.w  COPY_GROUP_COUNT.l,d7
                ble.b   $C1EAE8
                move.w  d7,d6
                lea     COPY_GROUP_SOURCE.l,a0
                lea     TRANSFORM_WORKSPACE.l,a1
                move.w  COPY_GROUP_INDEX.l,d0
                blt.b   $C1EAE8
                asl.w   #3,d0
                move.w  d0,d1
                add.w   d0,d0
                add.w   d1,d0
                adda.w  d0,a0
                lea     (a0),a2
                lea     (a1),a3
                movem.l (a0)+,d0-d5
                movem.l d0-d5,(a1)
                ; ADDA.W #$0018,A1; retain the original immediate encoding.
                dc.w    $D2FC,$0018
                dbra    d7,$C1EB18
                lea     (a3),a1
                move.w  d6,d7
                tst.w   (a3)
                blt.b   $C1EB54
                move.w  2(a3),d0
                andi.w  #$C000,d0
                cmpi.w  #$C000,d0
                bne.b   $C1EB54
