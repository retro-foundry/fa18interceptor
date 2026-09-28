#include "scene_positive_pose_tables.h"

int fa18_load_scene_positive_pose_tables(const FA18Hunks *h, FA18ScenePositivePoseTables *t) {
    const FA18HunkSegment *s;
    if (!h || !t || h->count <= FA18_SCENE_POSITIVE_TABLE_HUNK) return -1;
    s=&h->segments[FA18_SCENE_POSITIVE_TABLE_HUNK];
    if (!s->data || s->size < FA18_SCENE_POSITIVE_BYTE_OFFSET + FA18_SCENE_POSITIVE_BYTE_PAIRS*2u) return -1;
    t->word_pairs=s->data+FA18_SCENE_POSITIVE_WORD_OFFSET; t->byte_pairs=s->data+FA18_SCENE_POSITIVE_BYTE_OFFSET; return 0;
}
int fa18_resolve_scene_positive_pose(void *v, uint8_t index, FA18ScenePositivePoseInput *out) {
    FA18ScenePositivePoseResolver *r=v; int16_t words[FA18_SCENE_RECORD_TABLE_ENTRY_WORDS]; uint8_t selector;
    if (!r || !r->records || !r->tables || !r->tables->word_pairs || !r->tables->byte_pairs || !out || fa18_scene_record_table_a_entry(r->records,index,words)) return -1;
    selector=(uint8_t)words[2]; if (selector >= FA18_SCENE_POSITIVE_WORD_PAIRS || selector >= FA18_SCENE_POSITIVE_BYTE_PAIRS) return -1;
    for(unsigned i=0;i<5;i++) out->entry_words[i]=words[i];
    for(unsigned i=0;i<3;i++) out->tail_words[i]=words[i+5];
    out->grid_adjustment[0]=(int8_t)r->tables->byte_pairs[selector*2u]; out->grid_adjustment[1]=(int8_t)r->tables->byte_pairs[selector*2u+1];
    out->position_adjustment[0]=(int16_t)fa18_be16(r->tables->word_pairs+selector*4u); out->position_adjustment[1]=(int16_t)fa18_be16(r->tables->word_pairs+selector*4u+2); return 0;
}
