#ifndef FA18_GAME_SCENE_BOOTSTRAP_H
#define FA18_GAME_SCENE_BOOTSTRAP_H
#include "memory.h"

enum SceneBootstrapChild {
    BOOTSTRAP_CLEAR_STARTUP, BOOTSTRAP_ENABLE_RECORDS, BOOTSTRAP_CLEAR_BUFFERS,
    BOOTSTRAP_PREPARE_PLAYER, BOOTSTRAP_START_POSITION, BOOTSTRAP_SET_OBSERVER,
    BOOTSTRAP_PLACE_VIEW, BOOTSTRAP_BUILD_GATES, BOOTSTRAP_UPDATE_RECORDS,
    BOOTSTRAP_REFRESH_CONTEXT, BOOTSTRAP_RUN, BOOTSTRAP_FREE_VOICES,
    BOOTSTRAP_LOAD_MENU_TABLE
};
enum SceneBootstrapPhase {
    BOOTSTRAP_BUFFERS, BOOTSTRAP_RECORDS_CLEARED, BOOTSTRAP_POSITION,
    BOOTSTRAP_INITIALIZED, BOOTSTRAP_RESET_CALLBACK,
    BOOTSTRAP_FOLLOWUP_MODE, BOOTSTRAP_FOLLOWUP_CALLBACK
};
typedef struct {
    enum SceneBootstrapPhase phase;
    uint32_t value;
} SceneBootstrapEvent;
typedef struct {
    void (*consume)(void *context, enum SceneBootstrapChild child);
    void (*observe)(void *context, const SceneBootstrapEvent *event);
    void *context;
} SceneBootstrapHooks;
/* Complete C08F26 and its C0F920/C0F992 sequence callback wrappers. */
/* Source prefix through root placement and template-gate construction. The
 * caller still owes record update and context refresh before full bootstrap. */
void prepare_scene_storage(const SceneBootstrapHooks *hooks);
void bootstrap_scene(const SceneBootstrapHooks *hooks);
void reset_sequence_after_bootstrap(const SceneBootstrapHooks *hooks);
void begin_sequence_after_bootstrap(const SceneBootstrapHooks *hooks);
#endif
