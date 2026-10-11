; Byte-exact reconstruction of $C254E8-$C2554B (Hunk 20 +$814).
; Output: $C45854 = native-angle octant (0..7).

                org     $C254E8

ANGLE_SOURCE_SELECT_STATE equ $C45785
PRIMARY_ANGLE_WORD        equ $C45A96
ALTERNATE_ANGLE_LONG      equ $C45A8C
ANGLE_OCTANT_WIDTH        equ $0E10
ANGLE_OCTANT_2            equ $1C20
ANGLE_OCTANT_3            equ $2A30
ANGLE_OCTANT_4            equ $3840
ANGLE_OCTANT_5            equ $4650
ANGLE_OCTANT_6            equ $5460
ANGLE_OCTANT_7            equ $6270
ANGLE_OCTANT_RESULT       equ $C45854

classify_angle_octant:
                tst.b   ANGLE_SOURCE_SELECT_STATE
                beq.s   .alternate_source
                move.w  PRIMARY_ANGLE_WORD,d0
                bra.s   .classify
.alternate_source:
                move.l  ALTERNATE_ANGLE_LONG,d0

.classify:
                cmpi.w  #ANGLE_OCTANT_4,d0
                bge.s   .upper_half
                cmpi.w  #ANGLE_OCTANT_2,d0
                bge.s   .second_quarter
                cmpi.w  #ANGLE_OCTANT_WIDTH,d0
                bge.s   .octant_1
                moveq   #0,d1
                bra.s   .store
.octant_1:
                moveq   #1,d1
                bra.s   .store
.second_quarter:
                cmpi.w  #ANGLE_OCTANT_3,d0
                bge.s   .octant_3
                moveq   #2,d1
                bra.s   .store
.octant_3:
                moveq   #3,d1
                bra.s   .store
.upper_half:
                cmpi.w  #ANGLE_OCTANT_6,d0
                bge.s   .upper_quarter
                cmpi.w  #ANGLE_OCTANT_5,d0
                bge.s   .octant_5
                moveq   #4,d1
                bra.s   .store
.octant_5:
                moveq   #5,d1
                bra.s   .store
.upper_quarter:
                cmpi.w  #ANGLE_OCTANT_7,d0
                bge.s   .octant_7
                moveq   #6,d1
                bra.s   .store
.octant_7:
                moveq   #7,d1
.store:
                move.b  d1,ANGLE_OCTANT_RESULT
                rts
