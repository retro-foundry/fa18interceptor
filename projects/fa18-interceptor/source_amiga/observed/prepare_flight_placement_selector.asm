; Byte-exact dual-entry placement-selector prefix $C1CB14-$C1CB73.
; It selects a placement-list base/offset and derives the C459B2 comparison
; word.  The following C1CB74 record loop remains a separate source unit.

                org     $C1CB14

PLACEMENT_LIST_VARIANT           equ     $C45865
PLACEMENT_LIST_OFFSET             equ     $C459AA
PLACEMENT_LIST_OFFSET_A           equ     $C459AC
PLACEMENT_LIST_OFFSET_B           equ     $C459AE
PLACEMENT_LIST_AUX_FLAG           equ     $C45864
PLACEMENT_LIST_STATUS_FLAG        equ     $C458BB
PLACEMENT_DEPTH_COMPONENT         equ     $C45A78
PLACEMENT_DEPTH_LIMIT             equ     $00007FFF
PLACEMENT_DEPTH_SATURATED         equ     $7FFE
PLACEMENT_DEPTH_TABLE             equ     $C41130
PLACEMENT_COMPARISON_WORD         equ     $C459B2

prepare_flight_placement_selector:
                clr.b   PLACEMENT_LIST_VARIANT.l
                move.w  PLACEMENT_LIST_OFFSET_A.l,PLACEMENT_LIST_OFFSET.l
                bra.b   prepare_flight_placement_selector_common

prepare_flight_placement_selector_variant_b:
                move.b  #1,PLACEMENT_LIST_VARIANT.l
                move.w  PLACEMENT_LIST_OFFSET_B.l,PLACEMENT_LIST_OFFSET.l
prepare_flight_placement_selector_common:
                clr.b   PLACEMENT_LIST_AUX_FLAG.l
                clr.b   PLACEMENT_LIST_STATUS_FLAG.l
                move.l  PLACEMENT_DEPTH_COMPONENT.l,d0
                neg.l   d0
                cmpi.l  #PLACEMENT_DEPTH_LIMIT,d0
                ble.b   prepare_flight_placement_selector_depth_in_range
                move.w  #PLACEMENT_DEPTH_SATURATED,PLACEMENT_COMPARISON_WORD.l
                bra.b   prepare_flight_placement_selector_done
prepare_flight_placement_selector_depth_in_range:
                asr.w   #7,d0
                lea     PLACEMENT_DEPTH_TABLE.l,a2
                move.b  (a2,d0.w),d0
                ext.w   d0
                asl.w   #8,d0
                move.w  d0,PLACEMENT_COMPARISON_WORD.l
prepare_flight_placement_selector_done:
                ; Falls through into the C1CB74 placement-record loop.
