#pragma once

#ifndef NO_STD
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#endif


#define OSDI_VERSION_MAJOR_CURR 0
#define OSDI_VERSION_MINOR_CURR 3

#define PARA_TY_MASK 3
#define PARA_TY_REAL 0
#define PARA_TY_INT 1
#define PARA_TY_STR 2
#define PARA_KIND_MASK  (3 << 30)
#define PARA_KIND_MODEL (0 << 30)
#define PARA_KIND_INST  (1 << 30)
#define PARA_KIND_OPVAR (2 << 30)

#define ACCESS_FLAG_READ 0
#define ACCESS_FLAG_SET 1
#define ACCESS_FLAG_INSTANCE 4

#define JACOBIAN_ENTRY_RESIST_CONST 1
#define JACOBIAN_ENTRY_REACT_CONST 2
#define JACOBIAN_ENTRY_RESIST 4
#define JACOBIAN_ENTRY_REACT 8

#define CALC_RESIST_RESIDUAL 1
#define CALC_REACT_RESIDUAL 2
#define CALC_RESIST_JACOBIAN 4
#define CALC_REACT_JACOBIAN 8
#define CALC_NOISE 16
#define CALC_OP 32
#define CALC_RESIST_LIM_RHS 64
#define CALC_REACT_LIM_RHS 128
#define ENABLE_LIM 256
#define INIT_LIM 512
#define ANALYSIS_NOISE 1024
#define ANALYSIS_DC 2048
#define ANALYSIS_AC 4096
#define ANALYSIS_TRAN 8192
#define ANALYSIS_IC 16384
#define ANALYSIS_STATIC 32768
#define ANALYSIS_NODESET 65536

#define EVAL_RET_FLAG_LIM 1
#define EVAL_RET_FLAG_FATAL 2
#define EVAL_RET_FLAG_FINISH 4
#define EVAL_RET_FLAG_STOP 8


#define LOG_LVL_MASK 7
#define LOG_LVL_DEBUG 0
#define LOG_LVL_DISPLAY 1
#define LOG_LVL_INFO 2
#define LOG_LVL_WARN 3
#define LOG_LVL_ERR 4
#define LOG_LVL_FATAL 5
#define LOG_FMT_ERR 16

#define INIT_ERR_OUT_OF_BOUNDS 1



typedef struct OsdiLimFunction {
  char *name;
  uint32_t num_args;
  void *func_ptr;
}OsdiLimFunction;

typedef struct OsdiSimParas {
  char **names;
  double *vals;
  char **names_str;
  char **vals_str;
}OsdiSimParas;

typedef struct OsdiSimInfo {
    OsdiSimParas paras;
    double abstime;
    double *prev_solve;
    double *prev_state;
    double *next_state;
    uint32_t flags;
}OsdiSimInfo;

typedef union OsdiInitErrorPayload {
  uint32_t parameter_id;
}OsdiInitErrorPayload;

typedef struct OsdiInitError {
  uint32_t code;
  OsdiInitErrorPayload payload;
}OsdiInitError;

typedef struct OsdiInitInfo {
  uint32_t flags;
  uint32_t num_errors;
  OsdiInitError *errors;
}OsdiInitInfo;

typedef struct OsdiNodePair {
  uint32_t node_1;
  uint32_t node_2;
}OsdiNodePair;

typedef struct OsdiJacobianEntry {
  OsdiNodePair nodes;
  uint32_t react_ptr_off;
  uint32_t flags;
}OsdiJacobianEntry;

typedef struct OsdiNode {
  char *name;
  char *units;
  char *residual_units;
  uint32_t resist_residual_off;
  uint32_t react_residual_off;
  uint32_t resist_limit_rhs_off;
  uint32_t react_limit_rhs_off;
  bool is_flow;
}OsdiNode;

typedef struct OsdiParamOpvar {
  char **name;
  uint32_t num_alias;
  char *description;
  char *units;
  uint32_t flags;
  uint32_t len;
}OsdiParamOpvar;

typedef struct OsdiNoiseSource {
  char *name;
  OsdiNodePair nodes;
}OsdiNoiseSource;

typedef struct OsdiDescriptor {
  char *name;

  uint32_t num_nodes;
  uint32_t num_terminals;
  OsdiNode *nodes;

  uint32_t num_jacobian_entries;
  OsdiJacobianEntry *jacobian_entries;

  uint32_t num_collapsible;
  OsdiNodePair *collapsible;
  uint32_t collapsed_offset;

  OsdiNoiseSource *noise_sources;
  uint32_t num_noise_src;

  uint32_t num_params;
  uint32_t num_instance_params;
  uint32_t num_opvars;
  OsdiParamOpvar *param_opvar;

  uint32_t node_mapping_offset;
  uint32_t jacobian_ptr_resist_offset;

  uint32_t num_states;
  uint32_t state_idx_off;

  uint32_t bound_step_offset;

  uint32_t instance_size;
  uint32_t model_size;

  void *(*access)(void *inst, void *model, uint32_t id, uint32_t flags);

  void (*setup_model)(void *handle, void *model, OsdiSimParas *sim_params,
                                     OsdiInitInfo *res);
  void (*setup_instance)(void *handle, void *inst, void *model,
                                     double temperature, uint32_t num_terminals,
                                     OsdiSimParas *sim_params, OsdiInitInfo *res);

  uint32_t (*eval)(void *handle, void *inst, void *model, OsdiSimInfo *info);
  void (*load_noise)(void *inst, void *model, double freq, double *noise_dens);
  void (*load_residual_resist)(void *inst, void* model, double *dst);
  void (*load_residual_react)(void *inst, void* model, double *dst);
  void (*load_limit_rhs_resist)(void *inst, void* model, double *dst);
  void (*load_limit_rhs_react)(void *inst, void* model, double *dst);
  void (*load_spice_rhs_dc)(void *inst, void* model, double *dst,
                  double* prev_solve);
  void (*load_spice_rhs_tran)(void *inst, void* model, double *dst,
                  double* prev_solve, double alpha);
  void (*load_jacobian_resist)(void *inst, void* model);
  void (*load_jacobian_react)(void *inst, void* model, double alpha);
  void (*load_jacobian_tran)(void *inst, void* model, double alpha);
}OsdiDescriptor;



