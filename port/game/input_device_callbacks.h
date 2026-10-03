#ifndef FA18_INPUT_DEVICE_CALLBACKS_H
#define FA18_INPUT_DEVICE_CALLBACKS_H
#include "memory.h"
enum InputDeviceChild {
    IDC_PALETTE_FIRST, IDC_PALETTE_SECOND, IDC_PALETTE_STABLE, IDC_FADE,
    IDC_ADD_SERVER, IDC_REMOVE_SERVER, IDC_ALLOCATE_SIGNAL, IDC_FIND_TASK, IDC_SET_PORT_LIST,
    IDC_OPEN_TIMER, IDC_OPEN_INPUT, IDC_PREPARE_INPUT_PORT, IDC_SEND_INPUT_REQUEST, IDC_START_SERVER
};
enum InputDevicePhase {
    IDC_D0_BYTE, IDC_D1_BYTE, IDC_D2_BYTE, IDC_D0_WORD, IDC_D1_WORD,
    IDC_D0_LONG, IDC_D1_LONG, IDC_D0_FROM_D1, IDC_EXT_WORD, IDC_EXT_D0_LONG, IDC_EXT_D1_LONG,
    IDC_STORE_BYTE, IDC_STORE_WORD, IDC_STORE_LONG, IDC_TEST_BYTE, IDC_TEST_WORD,
    IDC_COMPARE_BYTE, IDC_COMPARE_WORD, IDC_A0, IDC_A1, IDC_AND_D0_WORD, IDC_AND_D1_WORD,
    IDC_SUB_D0_WORD, IDC_SUB_D1_WORD, IDC_ADD_D0_WORD, IDC_ADD_D0_BYTE, IDC_SUB_D0_BYTE,
    IDC_SUB_D2_BYTE, IDC_ASR_D0_WORD, IDC_ASR_D1_WORD, IDC_ASL_D0_LONG, IDC_ASL_D1_LONG,
    IDC_SUB_D1_LONG, IDC_ADD_MEMORY_WORD, IDC_SUB_MEMORY_WORD, IDC_ADD_MEMORY_LONG,
    IDC_PUSH_FIRST_PALETTE, IDC_PUSH_INPUT_PORT, IDC_PUSH_INPUT_REQUEST, IDC_READ_COUNTERS
};
typedef struct {
    int32_t (*consume)(void *context,enum InputDeviceChild child);
    void (*observe)(void *context,enum InputDevicePhase phase,uint32_t value,uint32_t other);
    void *context;
} InputDeviceHooks;
void advance_input_device_callback(gaddr frame,const InputDeviceHooks *h);
void install_input_device_callback(const InputDeviceHooks *h);
void remove_input_device_callback(const InputDeviceHooks *h);
void prepare_input_device_port(gaddr frame,const InputDeviceHooks *h);
void open_input_device_timer(gaddr frame,const InputDeviceHooks *h);
void open_input_device_request(const InputDeviceHooks *h);
void set_input_device_bounds(gaddr frame,const InputDeviceHooks *h);
void initialise_input_device_counters(gaddr frame,const InputDeviceHooks *h);
#endif
