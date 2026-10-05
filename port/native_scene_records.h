#ifndef FA18_NATIVE_SCENE_RECORDS_H
#define FA18_NATIVE_SCENE_RECORDS_H
#include "command_queue.h"

enum { FA18_NATIVE_SCENE_RECORDS=16, FA18_NATIVE_SCENE_RECORD_BYTES=512,
       FA18_NATIVE_SCENE_CLEAR_BYTES=164, FA18_NATIVE_SCENE_MAPPED_BYTES=186,
       FA18_NATIVE_SCENE_WORK_BYTES=32 };
typedef struct {
    FA18FlightCommandRecord *aircraft;
    FA18ContextCommandRecord *geometry;
    uint8_t byte_04,byte_0a,byte_0b,byte_20,byte_21,level_storage,byte_5f,byte_71,byte_7c,byte_7d;
    uint8_t *level;
    uint16_t word_06,word_08,word_0c,word_0e,word_60,angle_first,angle_third,
        word_6c,word_6e,word_78,word_7e,word_54,word_5a,word_b8;
    uint32_t long_10,long_3e,long_42,long_46,long_50,long_56,long_72;
    int16_t forward[3][3];
    /* Only unbound positions hold data here. Access through the field view;
     * mapped positions are zeroed after import, not duplicate scalar owners. */
    uint8_t unported[FA18_NATIVE_SCENE_RECORD_BYTES];
    PortFieldByte fields[FA18_NATIVE_SCENE_MAPPED_BYTES];
} FA18NativeSceneRecord;
typedef struct {
    FA18CommandInput *input;
    FA18FlightCommandRecord aircraft[FA18_NATIVE_SCENE_RECORDS];
    FA18ContextCommandRecord geometry[FA18_NATIVE_SCENE_RECORDS];
    FA18NativeSceneRecord records[FA18_NATIVE_SCENE_RECORDS];
    uint8_t work[FA18_NATIVE_SCENE_RECORDS][FA18_NATIVE_SCENE_WORK_BYTES];
} FA18NativeSceneRecords;

/* Import original record/work data into fresh caller-owned storage. Input's
 * root level becomes the canonical +$2B field of record zero. Geometry and
 * aircraft arrays are contiguous, suitable for the existing command owners.
 * Source arrays must not overlap storage. Keep all owners at stable addresses;
 * copying this bank does not rebind its pointers. No data is synthesized. */
int fa18_import_native_scene_records(FA18NativeSceneRecords *records,FA18CommandInput *input,
                                      const uint8_t *source,size_t source_bytes,
                                      const uint8_t *work,size_t work_bytes);
/* Bounded record data view for existing packet/asset consumers. Typed mapped
 * fields are read live, never from duplicated packed bytes. */
int fa18_read_native_scene_record(const FA18NativeSceneRecord *record,size_t offset,
                                    uint8_t *bytes,size_t count);
/* Apply bounded record/packet data to the same live typed owners in source
 * byte order; no copied record is installed as a second owner. */
int fa18_write_native_scene_record(FA18NativeSceneRecord *record,size_t offset,
                                     const uint8_t *bytes,size_t count);
/* Clear precisely 41 source longwords; preserve bytes +$A4..+$1FF.
 * Missing field owners return 0, retaining preceding stores. */
int fa18_clear_native_scene_record(FA18NativeSceneRecord *record);
/* Actual $C08F76..$C08FAA block: all 16 control prefixes, then all 16
 * workspace records. This is a complete clear block, not the full bootstrap. */
int fa18_clear_native_bootstrap_records(FA18NativeSceneRecords *records);
#endif
