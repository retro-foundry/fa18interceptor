/* Placement-cache ordering, complete $C1E540-$C1EBAE.
 * Source positions mix signed words, signed longs and packed cell offsets.
 * Preserve those widths and the original two-stage geometric tests. */
#include "placement_order.h"
#include "control_records.h"
#include "fault.h"
#include "fixed_math.h"
#include "globals.h"
#include "view_transform.h"

enum {
    CACHE = 0xC4F6CA, ANCHOR_INDEX = 0xC4FD5A,
    SUFFIX_INDEX = 0xC4FD5C, CLASSIFIED_COUNT = 0xC4FD5E,
    RECORD_BYTES = 24, FRONT = 0x8000, BEHIND = 0xC000
};

typedef PlacementOrderPoint OrderPoint;

static void visit(const PlacementOrderObserver *observer, PlacementOrderEvent event) {
    if (observer) observer->observe(observer->context, &event);
}

static int32_t add32(int32_t a, int32_t b) {
    return (int32_t)((uint32_t)a + (uint32_t)b);
}
static int32_t sub32(int32_t a, int32_t b) {
    return (int32_t)((uint32_t)a - (uint32_t)b);
}
static int32_t neg32(int32_t a) { return (int32_t)(0u - (uint32_t)a); }
static int16_t add16(int32_t a, int32_t b) { return (int16_t)((uint32_t)a + (uint32_t)b); }
static int16_t sub16(int32_t a, int32_t b) { return (int16_t)((uint32_t)a - (uint32_t)b); }
static int16_t neg16(int32_t a) { return (int16_t)(0u - (uint32_t)a); }
static int32_t asr32(int32_t a, unsigned count) {
    count &= 63u;
    return count >= 32 ? (a < 0 ? -1 : 0) : a >> count;
}
static int16_t asr16(int16_t a, unsigned count) {
    count &= 63u;
    return count >= 16 ? (a < 0 ? -1 : 0) : (int16_t)(a >> count);
}
static gaddr offset_word(gaddr base, int32_t offset) {
    return base + (gaddr)(int32_t)(int16_t)offset;
}

static OrderPoint position(gaddr entry, int16_t column, int16_t row, int cells,
                           const PlacementOrderObserver *observer) {
    uint16_t head = rd_u16(entry);
    OrderPoint p;
    gaddr record;
    int32_t out[3];
    if (head & 0x50u) {
        record = (head & 0x40u) ? workspace_record(head) : control_record(head);
        read_record_fields(record, &p.x, &p.y, &p.z);
        visit(observer, (PlacementOrderEvent){.phase=ORDER_FIELDS, .record=record});
        if (cells) {
            p.x = add32(p.x, cell_step(sub16(rd_u16(record + 6) & 255u, column)));
            p.z = add32(p.z, cell_step(sub16(rd_u16(record + 8) & 255u, row)));
        }
    } else {
        grid_relative_position(entry, head & 15u, out);
        p = (OrderPoint){out[0], out[1], out[2]};
        visit(observer, (PlacementOrderEvent){.phase=ORDER_PACKED_POSITION});
        if (cells) {
            uint16_t cell = rd_u16(entry + 14);
            p.x = add32(p.x, cell_step(sub16(cell >> 8, column)));
            p.z = add32(p.z, cell_step(sub16(cell & 255u, row)));
        }
    }
    return p;
}

/* SUB followed by BGE tests the operands' signed order, even on overflow.
 * The resulting magnitude still wraps before the subsequent signed CMP. */
static int outside_axis(int32_t value, int32_t origin, int32_t limit) {
    int32_t delta = sub32(value, origin);
    if (value < origin) delta = neg32(delta);
    return delta > limit;
}
static int outside(OrderPoint value, OrderPoint origin, int32_t limit) {
    return outside_axis(value.x, origin.x, limit)
        || outside_axis(value.y, origin.y, limit)
        || outside_axis(value.z, origin.z, limit);
}

static int eligible_reference(gaddr entry, const PlacementOrderObserver *observer) {
    gaddr descriptor = rd_u32(entry + 2);
    int32_t flags = rd_s32(descriptor + 4);
    int16_t at;
    uint8_t attribute;
    visit(observer, (PlacementOrderEvent){.phase=ORDER_DESCRIPTOR, .descriptor=descriptor, .value=(uint32_t)flags});
    if (flags <= 0) return 0;
    at = rd_s16((gaddr)flags);
    if (at >= 0) at = rd_s16((gaddr)flags + ((at & 0x4000) ? 2u : 4u));
    visit(observer, (PlacementOrderEvent){.phase=ORDER_DESCRIPTOR_WORD, .word=(uint16_t)at});
    if (at == -1) return 0;
    attribute = rd_u8((gaddr)flags + 7u + (at & 0x0FFFu));
    visit(observer, (PlacementOrderEvent){.phase=ORDER_DESCRIPTOR_FLAG, .word=(uint16_t)at & 0x0FFFu, .attribute=attribute});
    return (attribute & 0x10u) != 0;
}

/* The last ADD's signed branch sees the mathematical sum of its two
 * operands, while TST of the saved dot sees the wrapped 32-bit value. */
static int32_t dot_value(const int16_t normal[3], OrderPoint p) {
    int32_t yz = add32((int32_t)(int16_t)p.y * normal[1],
                      (int32_t)(int16_t)p.z * normal[2]);
    return add32(yz, (int32_t)(int16_t)p.x * normal[0]);
}
static int dot_negative(const int16_t normal[3], OrderPoint p) {
    int32_t yz = add32((int32_t)(int16_t)p.y * normal[1],
                      (int32_t)(int16_t)p.z * normal[2]);
    return (int64_t)yz + (int32_t)(int16_t)p.x * normal[0] < 0;
}

static OrderPoint from_candidate(gaddr vertex, gaddr record, unsigned shift,
                                 OrderPoint candidate) {
    return (OrderPoint){
        neg16(sub16(add16(asr16(rd_s16(vertex), shift), rd_s16(record + 12)), candidate.x)),
        neg32(sub32(add32(asr32(rd_s16(vertex + 2), shift), rd_s32(record + 16)), candidate.y)),
        neg16(sub16(add16(asr16(rd_s16(vertex + 4), shift), rd_s16(record + 14)), candidate.z))
    };
}

static OrderPoint from_view(gaddr vertex, gaddr record, unsigned shift, OrderPoint view) {
    int16_t x = add16(asr16(rd_s16(vertex), shift), rd_s16(record + 12));
    int16_t z = add16(asr16(rd_s16(vertex + 4), shift), rd_s16(record + 14));
    return (OrderPoint){neg32(sub32(x, view.x)),
        neg32(add32(add32(asr32(rd_s16(vertex + 2), shift), rd_s32(record + 16)), rd_s32(PROJECTION_Y))),
        neg32(sub32(z, view.z))};
}

static unsigned plane_relation(gaddr planes, uint16_t last_plane,
                                OrderPoint reference, OrderPoint candidate, OrderPoint view,
                                gaddr face_cursor, const PlacementOrderObserver *observer) {
    uint32_t i;
    for (i = 0; i <= last_plane; ++i, planes += 12) {
        int16_t normal[3] = {rd_s16(planes + 6), rd_s16(planes + 8), rd_s16(planes + 10)};
        int16_t x = add16(rd_s16(planes), reference.x);
        int16_t z = add16(rd_s16(planes + 4), reference.z);
        /* $C1E81C uses the candidate height directly on the plane route. */
        OrderPoint a = {neg16(sub16(x, candidate.x)), candidate.y, neg16(sub16(z, candidate.z))};
        OrderPoint b = {neg32(sub32(x, view.x)), neg32(rd_s32(PROJECTION_Y)), neg32(sub32(z, view.z))};
        int32_t candidate_dot = dot_value(normal, a);
        visit(observer, (PlacementOrderEvent){.phase=ORDER_PLANE, .record=planes + 12,
            .cursor=face_cursor, .coordinate=reference,
            .normal={normal[0],normal[1],normal[2]}});
        if (dot_negative(normal, b)) continue;
        if (candidate_dot >= 0) return FRONT;
    }
    return BEHIND;
}

/* $C1E8B8-$C1E95A: word-offset polygon groups, each closed by a negative
 * final offset, and the whole group list terminated by a negative first. */
static int behind_edge_groups(gaddr edges, gaddr record, unsigned shift,
                               const PlacementOrderObserver *observer) {
    for (;;) {
        int16_t first = rd_s16(edges);
        if (first < 0) return 0;
        for (;;) {
            int16_t offset = rd_s16(edges), next;
            int closing = offset < 0;
            gaddr vertex, other;
            int32_t nx, nz, px, pz;
            int16_t x, z;
            edges += 2;
            next = closing ? first : (int16_t)(rd_u16(edges) & 0x0FFFu);
            vertex = offset_word(record, add16(offset & 0x0FFFu, 0xA4));
            other = offset_word(record, add16(next, 0xA4));
            nx = asr32((int32_t)sub16(rd_s16(other + 4), rd_s16(vertex + 4)) * -256, 6);
            nz = asr32(neg32((int32_t)sub16(rd_s16(other), rd_s16(vertex)) * -256), 6);
            x = neg16(add16(add16(asr16(rd_s16(vertex), shift), rd_s16(record + 12)), rd_s16(PROJECTION_WORDS)));
            z = neg16(add16(add16(asr16(rd_s16(vertex + 4), shift), rd_s16(record + 14)), rd_s16(PROJECTION_WORDS + 4)));
            px = (int32_t)(int16_t)nx * x;
            pz = (int32_t)(int16_t)nz * z;
            visit(observer, (PlacementOrderEvent){.phase=ORDER_EDGE, .cursor=edges, .point=vertex,
                .normal={px,rd_s16(other + 2),add32(px,pz)}});
            if ((int64_t)px + pz >= 0) {
                if (!closing) {
                    do { offset = rd_s16(edges); edges += 2; } while (offset >= 0);
                    visit(observer, (PlacementOrderEvent){.phase=ORDER_EDGE_CURSOR, .cursor=edges});
                }
                break;
            }
            if (closing) return 1;
        }
    }
}

static unsigned indexed_relation(gaddr faces, uint16_t last_face, uint32_t metadata,
                                 gaddr record, unsigned shift, OrderPoint candidate, OrderPoint view,
                                 const PlacementOrderObserver *observer) {
    uint32_t i;
    for (i = 0; i <= last_face; ++i, faces += 4) {
        gaddr face = rd_u32(faces);
        gaddr a = offset_word(record, add16(rd_s16(face + 2), 0xA4));
        gaddr b = offset_word(record, add16(rd_s16(face + 4), 0xA4));
        gaddr c = offset_word(record, add16(rd_u16(face + 6) & 0x0FFFu, 0xA4));
        int16_t u[3] = {sub16(rd_s16(b), rd_s16(a)), sub16(rd_s16(b + 2), rd_s16(a + 2)), sub16(rd_s16(b + 4), rd_s16(a + 4))};
        int16_t v[3] = {sub16(rd_s16(c), rd_s16(a)), sub16(rd_s16(c + 2), rd_s16(a + 2)), sub16(rd_s16(c + 4), rd_s16(a + 4))};
        int32_t cross[3] = {
            asr32(sub32((int32_t)u[1] * v[2], (int32_t)v[1] * u[2]), 6),
            asr32(sub32((int32_t)u[2] * v[0], (int32_t)u[0] * v[2]), 6),
            asr32(sub32((int32_t)u[0] * v[1], (int32_t)u[1] * v[0]), 6)
        };
        int16_t normal[3] = {(int16_t)cross[0],(int16_t)cross[1],(int16_t)cross[2]};
        int32_t candidate_dot = dot_value(normal, from_candidate(a, record, shift, candidate));
        visit(observer, (PlacementOrderEvent){.phase=ORDER_TRIANGLE, .record=record, .cursor=faces + 4,
            .point=a, .normal={cross[0],cross[1],cross[2]}, .value=(uint32_t)((int32_t)u[1]*v[0])});
        if (dot_negative(normal, from_view(a, record, shift, view))) continue;
        if (candidate_dot >= 0) return FRONT;
        if ((int16_t)(metadata >> 16) < 0) {
            int16_t side[3];
            int16_t dy = rd_s16(a + 2);
            int32_t nz = asr32((int32_t)neg16(dy) * u[0], 6);
            int32_t nx = asr32((int32_t)dy * u[2], 6);
            OrderPoint p = from_candidate(a, record, shift, candidate);
            side[0] = (int16_t)nx; side[1] = 0; side[2] = (int16_t)nz;
            visit(observer, (PlacementOrderEvent){.phase=ORDER_TRIANGLE_SIDE, .normal={nx,0,nz}});
            if (!dot_negative(side, p)) return FRONT;
        }
    }
    return BEHIND;
}

static unsigned relation(gaddr reference_entry, gaddr entry,
                          int16_t column, int16_t row, OrderPoint view,
                          const PlacementOrderObserver *observer) {
    uint16_t head = rd_u16(entry);
    OrderPoint candidate;
    gaddr descriptor, faces, record;
    uint16_t header;
    if (head & 0x50u) {
        int32_t height;
        uint16_t index = head & 0xFF00u;
        record = (head & 0x40u)
            ? offset_word(WORKSPACE_RECORDS, (int16_t)index >> 3)
            : control_record(head);
        visit(observer, (PlacementOrderEvent){.phase=ORDER_SPECIAL_BASE,
            .record=(head & 0x40u) ? WORKSPACE_RECORDS : CONTROL_RECORDS});
        candidate = (OrderPoint){rd_s16(record + 12), rd_s32(record + 16), rd_s16(record + 14)};
        /* These special paths retain only the adjusted words. */
        candidate.x = add16(candidate.x, cell_step(sub16(rd_u16(record + 6) & 255u, column)));
        candidate.z = add16(candidate.z, cell_step(sub16(rd_u16(record + 8) & 255u, row)));
        descriptor = rd_u32(reference_entry + 2);
        faces = rd_u32(descriptor + 16);
        height = rd_s32(faces + 2);
        if ((head & 0x40u) || (rd_u8(record + 4) & 0xC0u)) {
            visit(observer, (PlacementOrderEvent){.phase=ORDER_SPECIAL_HEIGHT, .cursor=faces,
                .attribute=height < 0});
            if (height < 0) {
                gaddr base = (head & 0x40u) ? CONTROL_RECORDS : control_record(rd_u16(reference_entry));
                if (head & 0x40u) base += (rd_u16(reference_entry) & 0xFF00u) >> 3;
                height = add32((int32_t)((uint32_t)height & 0x7FFFFFFFu), rd_s32(base + 16));
                return neg32(rd_s32(PROJECTION_Y)) > height ? FRONT : BEHIND;
            }
            if (!(head & 0x40u)) { wr_u16(ERROR_CODE, 0x22); fault_hook(); return FRONT; }
        }
    } else candidate = position(entry, column, row, 1, observer);
    descriptor = rd_u32(reference_entry + 2);
    faces = rd_u32(descriptor + 16);
    header = rd_u16(faces); faces += 2;
    if ((int16_t)header >= 0) {
        int32_t out[3];
        OrderPoint ref;
        /* $C1E7F6 uses the preceding cell-step/index result, not the
         * reference header, for D1. Its low four bits are zero. */
        grid_relative_position(reference_entry, 0, out);
        visit(observer, (PlacementOrderEvent){.phase=ORDER_PACKED_POSITION});
        ref = (OrderPoint){out[0], out[1], out[2]};
        return plane_relation(rd_u32(faces + 4), header, ref, candidate, view, faces + 4, observer);
    }
    {
        uint32_t metadata = rd_u32(faces);
        unsigned shift;
        gaddr edges;
        faces += 4;
        record = control_record(rd_u16(reference_entry));
        shift = rd_u8(record + 0x7D) & 15u;
        edges = rd_u32(faces);
        visit(observer, (PlacementOrderEvent){.phase=ORDER_INDEXED_BEGIN, .record=record, .cursor=edges});
        {
            gaddr first = offset_word(record, add16(rd_s16(edges), 0xA4));
            int16_t height = asr16(rd_s16(first + 2), shift);
            if (neg16(rd_s32(PROJECTION_Y)) >= height && behind_edge_groups(edges, record, shift, observer)) {
                visit(observer, (PlacementOrderEvent){.phase=ORDER_INDEXED_RESTORE, .cursor=faces});
                return BEHIND;
            }
        }
        visit(observer, (PlacementOrderEvent){.phase=ORDER_INDEXED_RESTORE, .cursor=faces + 4});
        return indexed_relation(faces + 4, header & 0x3FFFu, metadata, record, shift, candidate, view, observer);
    }
}

static void copy_record(gaddr from, gaddr to, const PlacementOrderObserver *observer) {
    uint32_t words[6];
    unsigned i;
    for (i = 0; i < 6; ++i) words[i] = rd_u32(from + 4u * i);
    for (i = 0; i < 6; ++i) wr_u32(to + 4u * i, words[i]);
    visit(observer, (PlacementOrderEvent){.phase=ORDER_COPY, .value=words[5]});
}

void order_placement_cache(void) {
    order_placement_cache_observed(NULL);
}

void order_placement_cache_observed(const PlacementOrderObserver *observer) {
    gaddr entry = CACHE, reference;
    int16_t last = -1, first, column, row, classified = -1;
    OrderPoint anchor, view;
    while (rd_s16(entry) >= 0) { last = add16(last, 1); entry += RECORD_BYTES; }
    visit(observer, (PlacementOrderEvent){.phase=ORDER_SCAN, .entry=entry, .index=last});
    if (last < 1) return;
    entry -= RECORD_BYTES;
    visit(observer, (PlacementOrderEvent){.phase=ORDER_ENTRY, .entry=entry});
    while (!eligible_reference(entry, observer)) {
        last = sub16(last, 1);
        visit(observer, (PlacementOrderEvent){.phase=ORDER_SCAN, .entry=entry, .index=last});
        if (last < 0) return;
        entry -= RECORD_BYTES;
        visit(observer, (PlacementOrderEvent){.phase=ORDER_ENTRY, .entry=entry});
    }
    wr_s16(ANCHOR_INDEX, last);
    reference = entry;
    read_record_pair(reference, &column, &row);
    view = (OrderPoint){add32(neg16(rd_s16(PROJECTION_WORDS)), cell_step(sub16(rd_u16(GRID_ORIGIN_X) & 255u, column))),
                        0,
                       add32(neg16(rd_s16(PROJECTION_WORDS + 4)), cell_step(sub16(rd_u16(GRID_ORIGIN_Z) & 255u, row)))};
    /* The anchor route tests bit 4 only, as $C1E5D0 does. */
    if (rd_u16(reference) & 0x10u) {
        read_record_fields(control_record(rd_u16(reference)), &anchor.x, &anchor.y, &anchor.z);
        visit(observer, (PlacementOrderEvent){.phase=ORDER_FIELDS, .record=control_record(rd_u16(reference))});
    } else {
        int32_t out[3]; grid_relative_position(reference, rd_u16(reference) & 15u, out);
        anchor = (OrderPoint){out[0], out[1], out[2]};
        visit(observer, (PlacementOrderEvent){.phase=ORDER_PACKED_POSITION});
    }
    visit(observer, (PlacementOrderEvent){.phase=ORDER_ANCHOR, .coordinate=anchor});
    first = last;
    for (;;) {
        first = sub16(first, 1);
        visit(observer, (PlacementOrderEvent){.phase=ORDER_SCAN, .entry=entry, .index=first});
        if (first < 0) { first = add16(first, 1); break; }
        entry -= RECORD_BYTES;
        visit(observer, (PlacementOrderEvent){.phase=ORDER_ENTRY, .entry=entry});
        if (outside(position(entry, column, row, 1, observer), anchor, 0xC00)) {
            entry += RECORD_BYTES; first = add16(first, 1); break;
        }
    }
    wr_s16(SUFFIX_INDEX, first);
    visit(observer, (PlacementOrderEvent){.phase=ORDER_SCAN, .entry=entry, .index=first});
    wr_s16(CLASSIFIED_COUNT, -1);
    while (rd_s16(entry) >= 0) {
        visit(observer, (PlacementOrderEvent){.phase=ORDER_ENTRY, .entry=entry});
        if (entry != reference) {
            if (entry > reference) {
                OrderPoint p, ref;
                if (rd_u16(reference) & 0x10u) {
                    read_record_fields(control_record(rd_u16(reference)), &ref.x, &ref.y, &ref.z);
                    visit(observer, (PlacementOrderEvent){.phase=ORDER_FIELDS, .record=control_record(rd_u16(reference))});
                } else {
                    int32_t out[3]; grid_relative_position(reference, rd_u16(reference) & 15u, out);
                    ref = (OrderPoint){out[0],out[1],out[2]};
                    visit(observer, (PlacementOrderEvent){.phase=ORDER_PACKED_POSITION});
                }
                visit(observer, (PlacementOrderEvent){.phase=ORDER_REFERENCE_POINT, .coordinate=ref});
                p = position(entry, column, row, 1, observer);
                if (outside(p, ref, 0xA00)) break;
            }
            wr_u16(entry + 2, rd_u16(entry + 2) | relation(reference, entry, column, row, view, observer));
        }
        classified = add16(classified, 1);
        wr_s16(CLASSIFIED_COUNT, classified);
        entry += RECORD_BYTES;
    }
    visit(observer, (PlacementOrderEvent){.phase=ORDER_ENTRY, .entry=entry});
    visit(observer, (PlacementOrderEvent){.phase=ORDER_FINISH_COUNT, .index=classified});
    if (classified > 0)
        visit(observer, (PlacementOrderEvent){.phase=ORDER_PARTITION_BEGIN, .index=classified});
    if (classified > 0 && first >= 0) {
        gaddr source = CACHE + (gaddr)(int32_t)(int16_t)(first * RECORD_BYTES), out = source;
        gaddr scratch = WORKSPACES;
        uint32_t i;
        visit(observer, (PlacementOrderEvent){.phase=ORDER_PARTITION_SETUP, .record=source});
        for (i = 0; i <= (uint16_t)classified; ++i) { copy_record(source, scratch, observer); source += RECORD_BYTES; scratch += RECORD_BYTES; }
        scratch = WORKSPACES;
        for (i = 0; i <= (uint16_t)classified; ++i, scratch += RECORD_BYTES) {
            if (rd_s16(scratch) >= 0 && (rd_u16(scratch + 2) & 0xC000u) == BEHIND) {
                wr_u16(scratch + 2, rd_u16(scratch + 2) & 0x0FFFu);
                copy_record(scratch, out, observer); out += RECORD_BYTES;
                wr_s16(scratch, -1);
            }
        }
        copy_record(WORKSPACES + (gaddr)(int32_t)(int16_t)((last - first) * RECORD_BYTES), out, observer);
        out += RECORD_BYTES;
        scratch = WORKSPACES;
        for (i = 0; i <= (uint16_t)classified; ++i, scratch += RECORD_BYTES) {
            if (rd_s16(scratch) >= 0 && (rd_u16(scratch + 2) & 0xC000u) == FRONT) {
                wr_u16(scratch + 2, rd_u16(scratch + 2) & 0x0FFFu);
                copy_record(scratch, out, observer); out += RECORD_BYTES;
            }
        }
        visit(observer, (PlacementOrderEvent){.phase=ORDER_PARTITION_END, .record=out, .cursor=scratch});
    }
}
