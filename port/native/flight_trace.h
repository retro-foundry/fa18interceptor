#ifndef FA18_FLIGHT_TRACE_H
#define FA18_FLIGHT_TRACE_H
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/* Read-only diagnostics shared by independently running native/reference
 * executables. The reader supplies host bytes, never an emulated bus read. */
typedef const uint8_t *(*FA18FlightTraceReader)(void *context,uint32_t address,size_t size);
typedef struct {
    FILE *file;
    size_t bytes,budget;
    unsigned rows;
    int failed,message_fields,drawing_bands;
} FA18FlightTrace;
int fa18_flight_trace_open(FA18FlightTrace *trace,const char *path,size_t budget);
int fa18_flight_trace_write(FA18FlightTrace *trace,unsigned iteration,unsigned frame,
                            unsigned width,unsigned height,
                            FA18FlightTraceReader reader,void *context);
int fa18_flight_trace_close(FA18FlightTrace *trace);
#endif
