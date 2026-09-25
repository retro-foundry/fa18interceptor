; Byte-exact observed depth gate inside $C279D0, $C27A0C-$C27A31.
; The alternate fall-through at $C27A32 was not exercised by this capture.

                org     $C27A0C

PROJECTION_COMPONENT             equ     $C45A78
LINE_EMITTER_MODE_FLAG           equ     $C457A2
FLIGHT_UPDATE_DEPTH_REJECT       equ     $C27C44
FLIGHT_UPDATE_DEPTH_CONTINUE     equ     $C27A36

gate_c279d0_projection_component:
                move.l  #-$800,d2
                move.l  PROJECTION_COMPONENT.l,d1
                cmp.l   d2,d1
                blt.w   FLIGHT_UPDATE_DEPTH_REJECT
                move.b  #1,LINE_EMITTER_MODE_FLAG.l
                clr.w   -$22(a6)
                cmpi.l  #-$200,d1
                bge.b   FLIGHT_UPDATE_DEPTH_CONTINUE
