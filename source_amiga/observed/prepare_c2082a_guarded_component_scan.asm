; Byte-exact observed shared-frame setup and guards $C2082A-$C20849.

                org     $C2082A

COMPONENT_SCAN_POINTER          equ     $C45A32
COMPONENT_SCAN_DEPTH            equ     $C45A78
COMPONENT_SCAN_FLAG             equ     $C4586C
COMPONENT_SCAN_SKIP             equ     $C20826

prepare_c2082a_guarded_component_scan:
                movea.l COMPONENT_SCAN_POINTER.l,a3
                ; ADDA.W #$000A,A3; preserve the observed immediate encoding.
                dc.w    $d6fc,$000a
                adda.w  (a2)+,a3
                cmpi.l  #-$140,COMPONENT_SCAN_DEPTH.l
                ble.b   COMPONENT_SCAN_SKIP
                tst.b   COMPONENT_SCAN_FLAG.l
                bne.b   COMPONENT_SCAN_SKIP
