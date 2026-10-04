#ifndef FA18_EXEC_INTERRUPT_ADAPTER_H
#define FA18_EXEC_INTERRUPT_ADAPTER_H
#include <stdint.h>
int fa18_os_exec_interrupt_services_signature_matches(const uint8_t *);
int fa18_os_exec_interrupt_services_enable_reference(void);
int fa18_os_exec_irq_roots_step(void);
int fa18_os_exec_int_servers_step(void);
int fa18_os_exec_soft_interrupts_step(void);
int fa18_os_exec_irq_requires_outer_dispatch(uint32_t);
#endif
