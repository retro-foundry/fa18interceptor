; Byte-exact observed shifted-coordinate continuation $C1234E-$C1235D.
; It clears the midrange flag, then routes large local values at or above
; $0C80 to the common completion path; smaller values enter further classes.

                org     $C1234E

SHIFTED_COORDINATE_MIDRANGE_FLAG equ    $C4586C

clear_shifted_coordinate_midrange_flag:
                clr.b   SHIFTED_COORDINATE_MIDRANGE_FLAG
                move.w  -$2(a6),d0
                cmpi.w  #$C80,d0
                bge.b   $C12388
