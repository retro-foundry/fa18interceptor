#ifndef FA18_GLUE_PROJECTION_READOUTS_H
#define FA18_GLUE_PROJECTION_READOUTS_H
int glue_C2EC90(void); int glue_C2EC94(void); int glue_C2EC9C(void); int glue_C2ECA4(void);
int glue_C2ECA8(void); int glue_C32A44(void); int glue_C32AC8(void);
int glue_C33F70(void); int glue_C33F8A(void); int glue_C33FB4(void);
int glue_readout_x_limit(void); int glue_readout_y_limit(void);
int glue_C2EC90_complete_step(void); int glue_C2EC90_owns(uint32_t pc);
int glue_C2EC94_complete_step(void); int glue_C2EC94_owns(uint32_t pc);
int glue_C2EC9C_complete_step(void); int glue_C2EC9C_owns(uint32_t pc);
int glue_C2ECA4_complete_step(void); int glue_C2ECA4_owns(uint32_t pc);
int glue_C2ECA8_step(void); int glue_C2ECA8_owns(uint32_t pc);
int glue_C32A44_step(void); int glue_C32A44_owns(uint32_t pc);
int glue_C32AC8_step(void); int glue_C32AC8_owns(uint32_t pc);
int glue_C33F70_step(void); int glue_C33F70_owns(uint32_t pc);
int glue_C33F8A_step(void); int glue_C33F8A_owns(uint32_t pc);
int glue_C33FB4_step(void); int glue_C33FB4_owns(uint32_t pc);
#endif
