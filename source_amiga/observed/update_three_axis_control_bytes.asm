; Byte-exact bounded three-axis control update $C1B410-$C1B4CF.

                org     $C1B410

CONTROL_BITS_OFFSET              equ $65
CONTROL_AXIS_X_OFFSET            equ $28
CONTROL_AXIS_Y_OFFSET            equ $29
CONTROL_AXIS_Z_OFFSET            equ $2A
CONTROL_AXIS_X_MASK              equ $30
CONTROL_AXIS_Y_MASK              equ $C0
CONTROL_AXIS_Z_MASK              equ $0C
CONTROL_AXIS_X_INCREMENT_CODE    equ $10
CONTROL_AXIS_Y_INCREMENT_CODE    equ $40
CONTROL_AXIS_Z_INCREMENT_CODE    equ $04
CONTROL_AXIS_X_MIN               equ $EC
CONTROL_AXIS_X_MAX               equ $14
CONTROL_AXIS_Y_MIN               equ $EC
CONTROL_AXIS_Y_MAX               equ $14
CONTROL_AXIS_Z_MIN               equ $C4
CONTROL_AXIS_Z_MAX               equ $3C
CONTROL_AXIS_Z_STEP              equ 3

update_three_axis_control_bytes:
                move.b  CONTROL_BITS_OFFSET(a1),d2
                move.b  d2,d3
                andi.b  #CONTROL_AXIS_X_MASK,d2
                bne.b   .update_x
                bra.b   .store_x
.update_x:
                cmpi.b  #CONTROL_AXIS_X_INCREMENT_CODE,d2
                beq.b   .increment_x
                move.b  CONTROL_AXIS_X_OFFSET(a1),d4
                ble.b   .decrement_x_value
                moveq   #-1,d4
                bra.b   .clamp_x_min
.decrement_x_value:
                subq.b  #1,d4
.clamp_x_min:
                cmpi.b  #CONTROL_AXIS_X_MIN,d4
                bge.b   .store_x
                moveq   #CONTROL_AXIS_X_MIN,d4
                bra.b   .store_x
.increment_x:
                move.b  CONTROL_AXIS_X_OFFSET(a1),d4
                bge.b   .increment_x_value
                moveq   #1,d4
                bra.b   .clamp_x_max
.increment_x_value:
                addq.b  #1,d4
.clamp_x_max:
                cmpi.b  #CONTROL_AXIS_X_MAX,d4
                ble.b   .store_x
                moveq   #CONTROL_AXIS_X_MAX,d4
.store_x:
                move.b  d4,CONTROL_AXIS_X_OFFSET(a1)
                clr.b   d4

                move.b  d3,d2
                andi.b  #CONTROL_AXIS_Y_MASK,d3
                beq.b   .store_y
                cmpi.b  #CONTROL_AXIS_Y_INCREMENT_CODE,d3
                beq.b   .increment_y
                move.b  CONTROL_AXIS_Y_OFFSET(a1),d4
                ble.b   .decrement_y_value
                moveq   #-1,d4
                bra.b   .clamp_y_min
.decrement_y_value:
                subq.b  #1,d4
.clamp_y_min:
                cmpi.b  #CONTROL_AXIS_Y_MIN,d4
                bge.b   .store_y
                moveq   #CONTROL_AXIS_Y_MIN,d4
                bra.b   .store_y
.increment_y:
                move.b  CONTROL_AXIS_Y_OFFSET(a1),d4
                bge.b   .increment_y_value
                moveq   #1,d4
                bra.b   .clamp_y_max
.increment_y_value:
                addq.b  #1,d4
.clamp_y_max:
                cmpi.b  #CONTROL_AXIS_Y_MAX,d4
                ble.b   .store_y
                moveq   #CONTROL_AXIS_Y_MAX,d4
.store_y:
                move.b  d4,CONTROL_AXIS_Y_OFFSET(a1)
                clr.b   d4

                andi.b  #CONTROL_AXIS_Z_MASK,d2
                bne.b   .update_z
                bra.b   .store_z
.update_z:
                cmpi.b  #CONTROL_AXIS_Z_INCREMENT_CODE,d2
                beq.b   .increment_z
                move.b  CONTROL_AXIS_Z_OFFSET(a1),d4
                ble.b   .decrement_z_value
                moveq   #-CONTROL_AXIS_Z_STEP,d4
                bra.b   .clamp_z_min
.decrement_z_value:
                subq.b  #CONTROL_AXIS_Z_STEP,d4
.clamp_z_min:
                cmpi.b  #CONTROL_AXIS_Z_MIN,d4
                bge.b   .store_z
                moveq   #CONTROL_AXIS_Z_MIN,d4
                bra.b   .store_z
.increment_z:
                move.b  CONTROL_AXIS_Z_OFFSET(a1),d4
                bge.b   .increment_z_value
                moveq   #CONTROL_AXIS_Z_STEP,d4
                bra.b   .clamp_z_max
.increment_z_value:
                addq.b  #CONTROL_AXIS_Z_STEP,d4
.clamp_z_max:
                cmpi.b  #CONTROL_AXIS_Z_MAX,d4
                ble.b   .store_z
                moveq   #CONTROL_AXIS_Z_MAX,d4
.store_z:
                move.b  d4,CONTROL_AXIS_Z_OFFSET(a1)
                rts
