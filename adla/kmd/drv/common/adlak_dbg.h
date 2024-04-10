/*******************************************************************************
 * Copyright (C) 2022 Amlogic, Inc. All rights reserved.
 ******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file adlak_dbg.h
 * @brief
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   	Who				Date				Changes
 * ----------------------------------------------------------------------------
 * 1.00a shiwei.sun@amlogic.com	2022/07/21	Initial release
 * </pre>
 *
 ******************************************************************************/

#ifndef __ADLAK_DBG_H__
#define __ADLAK_DBG_H__

/***************************** Include Files *********************************/
#include "adlak_common.h"
struct adlak_task;

#ifdef __cplusplus
extern "C" {
#endif

/************************** Constant Definitions *****************************/

/**************************Global Variable************************************/

/**************************Type Definition and Structure**********************/

struct adlak_dbg_info {
    adlak_os_spinlock_t spinlock;

#define ADLAK_DBG_TIMESTAMP_MAX (4096)
    int start_idx;
    int end_idx;
#ifdef CONFIG_ADLAK_DEBUG_INNNER_TIME
    adlak_os_ktime_t start[ADLAK_DBG_TIMESTAMP_MAX];
    adlak_os_ktime_t finish[ADLAK_DBG_TIMESTAMP_MAX];
    char             identify_name[ADLAK_DBG_TIMESTAMP_MAX][256];
#endif

#ifdef CONFIG_ADLAK_DEBUG_INNNER_MODULE
#define ADLAK_MODULE_COUNT (14)
#define ADLAK_MODULE_DATA_MAX (0x10000)
    void *vaddr_module_data;
#endif
};

/************************** Function Prototypes ******************************/

int adlak_dbg_inner_init(struct adlak_context *context);

void adlak_dbg_inner_deinit(struct adlak_context *context);
void adlak_dbg_inner_reset(struct adlak_context *context);
void adlak_dbg_inner_update(struct adlak_context *context, char *identify_name);
void adlak_dbg_inner_dump_info(struct adlak_context *context);

void adlak_dbg_dump_module_read_data(struct adlak_context *context);
void adlak_dbg_dump_module_dump_data(struct adlak_context *context);
#ifdef __cplusplus
}
#endif

#endif /* __ADLAK_DBG_H__ end define*/
