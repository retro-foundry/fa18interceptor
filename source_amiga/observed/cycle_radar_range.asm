; Byte-exact raw-$13 radar-range command $C1B1C8-$C1B21B.
; Reached by sealed run003 documented R event.

                org     $C1B1C8

COMMAND_SIDE_EFFECT             equ $C33186
COMMAND_REQUEST_FLAGS           equ $C4599A
RADAR_RANGE_REQUEST_BIT          equ 6
CONTROL_RECORD_BASE             equ $C46184
CONTROL_RECORD_OFFSET            equ $C458DE
RADAR_RANGE_FIELD_OFFSET         equ $63
RADAR_RANGE_LOW_NIBBLE_MASK      equ $0F
RADAR_RANGE_CLEAR_MASK           equ $F0
RADAR_RANGE_NEAR                 equ 9
RADAR_RANGE_MEDIUM               equ 11
RADAR_RANGE_FAR                  equ 13
RADAR_DISPLAY_UPDATE_MODE        equ $C4583B
RADAR_DISPLAY_UPDATE_MODE_VALUE  equ 3
SHARED_COMMAND_FALLBACK          equ $C1C23C

cycle_radar_range:
                jsr     COMMAND_SIDE_EFFECT.l
                bset.b  #RADAR_RANGE_REQUEST_BIT,COMMAND_REQUEST_FLAGS.l
                lea.l   CONTROL_RECORD_BASE.l,a0
                adda.w  CONTROL_RECORD_OFFSET.l,a0
                move.b  RADAR_RANGE_FIELD_OFFSET(a0),d4
                andi.b  #RADAR_RANGE_LOW_NIBBLE_MASK,d4
                cmpi.b  #RADAR_RANGE_NEAR,d4
                beq.s   select_radar_far
                cmpi.b  #RADAR_RANGE_MEDIUM,d4
                beq.s   select_radar_near
                move.b  #RADAR_RANGE_MEDIUM,d4
                bra.s   store_radar_range
select_radar_far:
                move.b  #RADAR_RANGE_FAR,d4
                bra.s   store_radar_range
select_radar_near:
                move.b  #RADAR_RANGE_NEAR,d4
store_radar_range:
                andi.b  #RADAR_RANGE_CLEAR_MASK,RADAR_RANGE_FIELD_OFFSET(a0)
                or.b    d4,RADAR_RANGE_FIELD_OFFSET(a0)
                move.b  #RADAR_DISPLAY_UPDATE_MODE_VALUE,RADAR_DISPLAY_UPDATE_MODE.l
                bra.w   SHARED_COMMAND_FALLBACK
