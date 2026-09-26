; Byte-exact alternate map-control detail component route $C2AEF8-$C2AEFB.
; Completes the component preparation before the packet header is read.

                org     $C2AEF8

complete_map_control_detail_component_route:
                swap    d0
                move.w  d1,d0
