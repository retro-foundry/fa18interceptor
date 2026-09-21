; Byte-exact guarded stack-packet publisher $C17EF2-$C17F8B.
; The $C3318E bounded command-side-effect trace enters and returns from it.

                org     $C17EF2

PACKET_GUARD_POINTER             equ $C0A448
PACKET_SLOT_00                   equ $C50B7C
PACKET_SLOT_04                   equ $C50B84
PACKET_SLOT_08                   equ $C50B8C
PACKET_SLOT_0C                   equ $C50B94
PACKET_SLOT_10                   equ $C50B9C
PACKET_SLOT_14                   equ $C50BA4
PACKET_SLOT_18                   equ $C50BAC
PACKET_SLOT_1C                   equ $C50BB4
PACKET_SLOT_28                   equ $C50BC4
PACKET_RECORD_OFFSET_2C          equ $2C
PACKET_RECORD_OFFSET_34          equ $34

CALL_C17B08                      equ $C17B08
CALL_C17B2C                      equ $C17B2C

publish_guarded_command_packet:
                link.w  a6,#0
                tst.l   PACKET_GUARD_POINTER.l
                beq.w   .return
                moveq   #3,d0
                move.l  d0,-(a7)
                bsr.w   CALL_C17B08
                addq.l  #4,a7
                move.l  $24(a6),PACKET_SLOT_00.l
                move.l  $20(a6),PACKET_SLOT_04.l
                moveq   #$10,d0
                move.l  8(a6),d1
                asl.l   d0,d1
                move.l  d1,PACKET_SLOT_08.l
                move.l  $C(a6),d1
                asl.l   d0,d1
                move.l  d1,PACKET_SLOT_0C.l
                move.l  $10(a6),PACKET_SLOT_10.l
                move.l  $14(a6),d1
                asl.l   d0,d1
                move.l  d1,PACKET_SLOT_14.l
                move.l  $18(a6),d1
                asl.l   d0,d1
                move.l  d1,PACKET_SLOT_18.l
                move.l  $1C(a6),PACKET_SLOT_1C.l
                move.l  $28(a6),PACKET_SLOT_28.l
                movea.l PACKET_GUARD_POINTER.l,a0
                moveq   #0,d0
                move.l  d0,PACKET_RECORD_OFFSET_34(a0)
                moveq   #1,d0
                move.l  d0,PACKET_RECORD_OFFSET_2C(a0)
                clr.l   -(a7)
                moveq   #3,d0
                move.l  d0,-(a7)
                moveq   #4,d0
                move.l  d0,-(a7)
                bsr.w   CALL_C17B2C
                lea.l   $C(a7),a7
.return:
                unlk    a6
                rts
