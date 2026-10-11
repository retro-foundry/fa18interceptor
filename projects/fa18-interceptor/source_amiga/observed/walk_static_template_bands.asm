; Byte-exact static-template band walker $C1D330-$C1D3F3.
; It combines signed segment-65 control bytes with two live selector terms,
; calls select_static_template_stream for each accepted band, and advances the
; mutable band workspace by $600 bytes.  Axis and physical-world meanings of
; the selector terms are not established here.

                org     $C1D330

TEMPLATE_WORKSPACE_BASE    equ     $C48390
TEMPLATE_APPEND_ENABLE     equ     $C45864
TEMPLATE_BAND_STATUS       equ     $C45AD4
TEMPLATE_BAND_ERROR        equ     $C4599E
TEMPLATE_CONTROL_TRANSLATE equ     -$3A
TEMPLATE_ROW_TERM          equ     -$26
TEMPLATE_GROUP_TERM        equ     -$28

walk_static_template_bands:
                lea.l   TEMPLATE_WORKSPACE_BASE.l,a3
                moveq   #$e,d4
.next_control_byte:
                move.b  (a0)+,d0
                blt.w   .finish
                subq.w  #1,d4
                blt.w   .too_many_bands
                cmpi.b  #$14,d0
                bgt.b   .invalid_control_byte
                ext.w   d0
                movea.l TEMPLATE_CONTROL_TRANSLATE(a6),a2
                move.b  (a2,d0.w),d0
                ext.w   d0
                add.w   d0,d0
                lea.l   $C1D764.l,a2
                tst.b   TEMPLATE_APPEND_ENABLE.l
                bne.b   .append_enabled_bound
                move.w  #$1e,d3
                bra.b   .have_bound
.append_enabled_bound:
                move.w  #$7f,d3
.have_bound:
                move.b  (a2,d0.w),d2
                ext.w   d2
                move.w  TEMPLATE_ROW_TERM(a6),d1
                add.w   d2,d1
                blt.b   .clamp_row_high
                cmp.w   d3,d1
                bgt.b   .clamp_row_low
.have_group_delta:
                move.b  $1(a2,d0.w),d2
                ext.w   d2
                move.w  TEMPLATE_GROUP_TERM(a6),d0
                add.w   d2,d0
                blt.b   .clamp_group_high
                cmp.w   d3,d0
                ble.b   .select_band_stream
                bra.b   .clamp_group_low
.clamp_row_high:
                move.w  d3,d1
                bra.b   .have_group_delta
.clamp_row_low:
                move.w  #0,d1
                bra.b   .have_group_delta
.clamp_group_high:
                move.w  d3,d0
                bra.b   .select_band_stream
.clamp_group_low:
                move.w  #0,d0
                bra.b   .select_band_stream
.unreached_control_fallback:
                move.w  #$10,d1
                move.w  #0,d0
.select_band_stream:
                lea.l   (a3),a1
                bsr.w   $C1D3F4
                move.b  (a0)+,d0
                ext.w   d0
                adda.w  d0,a0
                dc.w    $D6FC,$0600       ; adda.w #$600,a3; preserve original immediate form
                bra.w   .next_control_byte
.invalid_control_byte:
                move.w  #$c,TEMPLATE_BAND_ERROR.l
                jsr     $C06C02.l
                bra.b   .invalid_control_byte
.too_many_bands:
                move.w  #$d,TEMPLATE_BAND_ERROR.l
                jsr     $C06C02.l
                bra.b   .too_many_bands
.finish:
                move.w  #$52,TEMPLATE_BAND_STATUS.l
                jmp     $C1DC08.l
