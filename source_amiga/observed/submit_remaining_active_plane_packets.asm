; Byte-exact continuation of the active-plane packet at $C2FD8C.
; The preceding first-plane trigger is $C2FDF0; this slice contains the
; remaining three trigger paths through $C2FEDA. Packet/caller semantics stay
; deliberately unassigned.
                org $C2FDF4
CUSTOM equ $DFF000
DMACONR equ $002
BLTCON0 equ $040
BLTCPT equ $048
BLTDPT equ $054
BLTSIZE equ $058
PLANE2_BUSY_COUNT equ $C4591C
PLANE3_BUSY_COUNT equ $C45920
PLANE4_BUSY_COUNT equ $C45924
PLANE3_SIZE_SELECT equ $C4589B
remaining_active_plane_packets:
; Second table entry: busy polling updates the observed $C4591C longword.
 move.l 4(a2),d4
 addi.l #$28,d4
 move.w #$3fa,d2
.plane2_wait:
 btst.b #6,DMACONR(a0)
 beq.b .plane2_ready
 addq.w #1,PLANE2_BUSY_COUNT.l
 bra.b .plane2_wait
.plane2_ready:
 move.l PLANE2_BUSY_COUNT.l,d0
 move.w d0,d3
 swap d0
 cmp.w d3,d0
 ble.b .plane2_keep_low
 move.w d0,d3
.plane2_keep_low:
 clr.w d0
 swap d0
 move.w d3,d0
 move.l d0,PLANE2_BUSY_COUNT.l
 move.w d2,BLTCON0(a0)
 move.l d4,BLTCPT(a0)
 move.l d4,BLTDPT(a0)
 move.w d6,BLTSIZE(a0)
; Third table entry: $C4589B selects the observed alternate BLTCON0 word.
 move.l 8(a2),d4
 addi.l #$28,d4
 move.w #$100,d2
 tst.b PLANE3_SIZE_SELECT.l
 beq.b .plane3_size_ready
 move.w #$3fa,d2
.plane3_size_ready:
 btst.b #6,DMACONR(a0)
 beq.b .plane3_ready
 addq.w #1,PLANE3_BUSY_COUNT.l
 bra.b .plane3_size_ready
.plane3_ready:
 move.l PLANE3_BUSY_COUNT.l,d0
 move.w d0,d3
 swap d0
 cmp.w d3,d0
 ble.b .plane3_keep_low
 move.w d0,d3
.plane3_keep_low:
 clr.w d0
 swap d0
 move.w d3,d0
 move.l d0,PLANE3_BUSY_COUNT.l
 move.w d2,BLTCON0(a0)
 move.l d4,BLTCPT(a0)
 move.l d4,BLTDPT(a0)
 move.w d6,BLTSIZE(a0)
; Fourth table entry.
 move.l 12(a2),d4
 addi.l #$28,d4
 move.w #$100,d2
.plane4_wait:
 btst.b #6,DMACONR(a0)
 beq.b .plane4_ready
 addq.w #1,PLANE4_BUSY_COUNT.l
 bra.b .plane4_wait
.plane4_ready:
 move.l PLANE4_BUSY_COUNT.l,d0
 move.w d0,d3
 swap d0
 cmp.w d3,d0
 ble.b .plane4_keep_low
 move.w d0,d3
.plane4_keep_low:
 clr.w d0
 swap d0
 move.w d3,d0
 move.l d0,PLANE4_BUSY_COUNT.l
 move.w d2,BLTCON0(a0)
 move.l d4,BLTCPT(a0)
 move.l d4,BLTDPT(a0)
 move.w d6,BLTSIZE(a0)
