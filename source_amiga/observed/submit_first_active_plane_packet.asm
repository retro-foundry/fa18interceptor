; Byte-exact first active-plane submission within $C2FD8C packet.
                org $C2FD8C
TABLE equ $C456B6
SIZE_INPUT equ $C45984
CUSTOM equ $DFF000
DMACONR equ $002
BLTCON0 equ $040
BLTCON1 equ $042
BLTAFWM equ $044
BLTALWM equ $046
BLTCPT equ $048
BLTDPT equ $054
BLTSIZE equ $058
BLTCMOD equ $060
BLTBMOD equ $062
BLTAMOD equ $064
BLTDMOD equ $066
BLTADAT equ $074
first_active_plane_packet:
 movea.l TABLE.l,a2
 lea CUSTOM.l,a0
 move.w SIZE_INPUT.l,d6
 asl.w #6,d6
 addi.w #$14,d6
 move.l (a2),d4
 addi.l #$28,d4
 move.w #$100,d2
 moveq #-1,d5
.wait:
 btst.b #6,DMACONR(a0)
 beq.b .go
 nop
 nop
 bra.b .wait
.go:
 move.w d2,BLTCON0(a0)
 move.w #0,BLTCON1(a0)
 move.w d5,BLTADAT(a0)
 move.w d5,BLTAFWM(a0)
 move.w d5,BLTALWM(a0)
 move.w #1,BLTBMOD(a0)
 move.w #1,BLTCMOD(a0)
 move.w #1,BLTDMOD(a0)
 move.l d4,BLTCPT(a0)
 move.l d4,BLTDPT(a0)
 move.w d6,BLTSIZE(a0)
