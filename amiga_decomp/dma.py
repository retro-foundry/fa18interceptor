"""Pinned Engine9000 DMA frame-view ctypes ABI; no game context required."""
import ctypes as C

class Info(C.Structure):
    _fields_ = [('version', C.c_uint32), ('frameSelect', C.c_uint32),
                ('frameNumber', C.c_int32), ('recordToggle', C.c_int32),
                ('hposCount', C.c_int32), ('vposCount', C.c_int32),
                ('dmaHoffset', C.c_int32), ('recordCount', C.c_uint32),
                ('debugDmaEnabled', C.c_uint32)]

class View(C.Structure):
    _fields_ = [('records', C.c_void_p), ('info', Info),
                ('visibleWidth', C.c_int32), ('visibleHeight', C.c_int32),
                ('renderWidth', C.c_int32), ('renderHeight', C.c_int32),
                ('visibleOffsetX', C.c_int32), ('visibleOffsetY', C.c_int32),
                ('dhposWrap', C.c_int32), ('dhposScale', C.c_int32)]

class Record(C.Structure):
    _fields_ = [('hpos', C.c_int), ('vpos', C.c_int), ('dhpos', C.c_int),
                ('dhpos_abs', C.c_int), ('reg', C.c_uint16), ('dat', C.c_uint64),
                ('size', C.c_uint16), ('addr', C.c_uint32), ('evt', C.c_uint32),
                ('evt2', C.c_uint32), ('evtdata', C.c_uint32),
                ('evtdataset', C.c_bool), ('type', C.c_int16), ('extra', C.c_uint16),
                ('intlev', C.c_int8), ('ipl', C.c_int8), ('ipl2', C.c_int8),
                ('cf_reg', C.c_uint16), ('cf_dat', C.c_uint16), ('cf_addr', C.c_uint16),
                ('ciareg', C.c_int), ('ciamask', C.c_int), ('ciarw', C.c_bool),
                ('ciaphase', C.c_int), ('ciavalue', C.c_uint16), ('end', C.c_bool)]

