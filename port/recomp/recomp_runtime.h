#ifndef FA18_RECOMP_RUNTIME_H
#define FA18_RECOMP_RUNTIME_H

/* Runtime contract for the generated translation (tools/recomp/recomp.py).
 *
 * Each original routine is one C function `fa18_fn_XXXXXX(int entry)`. Every
 * leader (routine start, branch target, instruction after a call) is a label
 * and a registered entry, so the dispatcher can resume a routine anywhere the
 * 68000 PC can legitimately be. All CPU state lives in Musashi's register
 * file; a generated routine can therefore stop at any label and the
 * interpreter or another routine carries on.
 *
 * Per instruction, the data operation runs through Musashi's handler for that
 * opcode (identical semantics and base cycle cost); the translation owns the
 * control flow. Return codes:
 *   FA18_RET            RTS/RTR executed; REG_PC is the popped address.
 *   FA18_EXIT_DISPATCH  control left the routine; dispatch again at REG_PC.
 *   FA18_EXIT_INTERP    the interpreter must execute the instruction at REG_PC
 *                       (slice budget exhausted, invalidated code, or an
 *                       instruction the translation hands back: RTE, STOP,
 *                       TRAP, line A/F, undecodable bytes). */

#include <stdint.h>

#ifdef FA18_RECOMP_GENERATED
#include "m68kcpu.h"
#include "m68kops.h"
#endif

enum { FA18_RET = 0, FA18_EXIT_DISPATCH = 1, FA18_EXIT_INTERP = 2 };

typedef int (*FA18RecompFn)(int entry);
typedef struct { FA18RecompFn fn; uint32_t entry; } FA18RecompFunction;
typedef struct { uint32_t pc; uint32_t function; uint32_t label; } FA18RecompEntry;
typedef struct { uint32_t function; uint32_t start, end; } FA18RecompSpan;

extern const FA18RecompFunction fa18_recomp_functions[];
extern const int fa18_recomp_function_count;
extern const FA18RecompEntry fa18_recomp_entries[];
extern const int fa18_recomp_entry_count;
extern const FA18RecompSpan fa18_recomp_spans[];
extern const int fa18_recomp_span_count;

extern int fa18_recomp_abort;

typedef struct {
    uint64_t dispatches;          /* hook entries into generated code */
    uint64_t generated_cycles;    /* CPU cycles spent in generated code */
    uint64_t interpreted_cycles;  /* CPU cycles spent in Musashi */
    uint64_t interpreted_game;    /* interpreted instructions in game RAM */
    uint64_t code_writes;         /* writes that hit translated bytes */
    int disabled_functions;
} FA18RecompStats;

extern FA18RecompStats fa18_recomp_stats;

/* enabled=0 runs the whole program on the interpreter (differential baseline). */
void fa18_recomp_init(int enabled);
void fa18_recomp_note_write(uint32_t address, int size);
void fa18_recomp_begin_slice(void);
int fa18_recomp_pending_cycles(void);
int fa18_recomp_invoke(int function, int label, uint32_t pc);
int fa18_recomp_call_dynamic(void);
/* Game-RAM PCs the interpreter executed, for the next generator run. */
int fa18_recomp_write_fallback_log(const char *path);

#ifdef FA18_RECOMP_GENERATED

extern int64_t fa18_cycle_origin, fa18_next_event;

/* Before every instruction: hand back to the dispatcher when chipset work is
 * due (it resumes here after servicing) or the routine was invalidated. */
#define FA18_EXEC(pc, op)                                                        \
    do {                                                                         \
        if (fa18_cycle_origin - GET_CYCLES() >= fa18_next_event || fa18_recomp_abort) { \
            REG_PC = (pc);                                                       \
            return FA18_EXIT_INTERP;                                             \
        }                                                                        \
        REG_PPC = (pc);                                                          \
        REG_PC = (pc) + 2;                                                       \
        REG_IR = (op);                                                           \
        m68ki_instruction_jump_table[(op)]();                                    \
        USE_CYCLES(CYC_INSTRUCTION[(op)]);                                       \
    } while (0)

#define FA18_CHECK(pc) ((void)0)

#define FA18_OP(pc, op, next)                                        \
    do {                                                             \
        FA18_EXEC(pc, op);                                           \
        if (REG_PC != (next)) return FA18_EXIT_DISPATCH;             \
    } while (0)

#define FA18_BCC(pc, op, next, target, label)                        \
    do {                                                             \
        FA18_EXEC(pc, op);                                           \
        if (REG_PC == (target)) goto label;                          \
        if (REG_PC != (next)) return FA18_EXIT_DISPATCH;             \
    } while (0)

#define FA18_BCC_OUT(pc, op, next) FA18_OP(pc, op, next)

#define FA18_JUMP(pc, op, target, label)                             \
    do {                                                             \
        FA18_EXEC(pc, op);                                           \
        if (REG_PC != (target)) return FA18_EXIT_DISPATCH;           \
        goto label;                                                  \
    } while (0)

#define FA18_JUMP_OUT(pc, op)                                        \
    do {                                                             \
        FA18_EXEC(pc, op);                                           \
        return FA18_EXIT_DISPATCH;                                   \
    } while (0)

#define FA18_CALL(pc, op, next, id, label)                           \
    do {                                                             \
        int r_;                                                      \
        FA18_EXEC(pc, op);                                           \
        r_ = fa18_recomp_invoke((id), (label), REG_PC);              \
        if (r_ != FA18_RET) return r_;                               \
        if (REG_PC != (next)) return FA18_EXIT_DISPATCH;             \
    } while (0)

#define FA18_CALL_DYNAMIC(pc, op, next)                              \
    do {                                                             \
        int r_;                                                      \
        FA18_EXEC(pc, op);                                           \
        r_ = fa18_recomp_call_dynamic();                             \
        if (r_ != FA18_RET) return r_;                               \
        if (REG_PC != (next)) return FA18_EXIT_DISPATCH;             \
    } while (0)

#define FA18_RETURN(pc, op)                                          \
    do {                                                             \
        FA18_EXEC(pc, op);                                           \
        return FA18_RET;                                             \
    } while (0)

#define FA18_INTERP(pc)                                              \
    do {                                                             \
        REG_PC = (pc);                                               \
        return FA18_EXIT_INTERP;                                     \
    } while (0)

#endif
#endif
