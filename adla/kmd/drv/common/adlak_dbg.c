/*******************************************************************************
 * Copyright (C) 2022 Amlogic, Inc. All rights reserved.
 ******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file adlak_dbg.c
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

/***************************** Include Files *********************************/

#include "adlak_dbg.h"

#include "adlak_io.h"
#include "adlak_submit.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

int adlak_dbg_inner_init(struct adlak_context *context) {
    context->dbg_info = adlak_os_zalloc(sizeof(struct adlak_dbg_info), ADLAK_GFP_KERNEL);
    if (!context->dbg_info) {
        ASSERT(0);
    }

    adlak_os_spinlock_init(&context->dbg_info->spinlock);

#ifdef CONFIG_ADLAK_DEBUG_INNNER_MODULE
    context->dbg_info->vaddr_module_data =
        adlak_os_malloc(ADLAK_MODULE_COUNT * ADLAK_MODULE_DATA_MAX * sizeof(uint32_t), 0);

#endif
    return ERR(NONE);
}

void adlak_dbg_inner_deinit(struct adlak_context *context) {
    if (context->dbg_info) {
#ifdef CONFIG_ADLAK_DEBUG_INNNER_MODULE
        adlak_os_free(context->dbg_info->vaddr_module_data);
#endif
        adlak_os_spinlock_destroy(&context->dbg_info->spinlock);
        adlak_os_free(context->dbg_info);
    }
}

void adlak_dbg_inner_reset(struct adlak_context *context) {
#ifdef CONFIG_ADLAK_DEBUG_INNNER_TIME
    struct adlak_dbg_info *info = context->dbg_info;
    if (info) {
        info->start_idx = 0;
        info->end_idx   = 0;
    }
#endif
}

void adlak_dbg_inner_update(struct adlak_context *context, char *identify_name) {
#ifdef CONFIG_ADLAK_DEBUG_INNNER_TIME
    struct adlak_dbg_info *info = context->dbg_info;
    if (!context->dbg_info) {
        return;
    }
    if (info->start_idx >= ADLAK_DBG_TIMESTAMP_MAX) {
        info->start_idx = 0;
    }

    if (NULL != identify_name) {
        adlak_os_snprintf(info->identify_name[info->start_idx], sizeof(info->identify_name[0]),
                          "%s", identify_name);
    }
    info->start[info->start_idx] = adlak_os_ktime_get();
    info->start_idx++;
#endif
}

void adlak_dbg_inner_dump_info(struct adlak_context *context) {
#ifdef CONFIG_ADLAK_DEBUG_INNNER_TIME
    struct adlak_dbg_info *info = context->dbg_info;
    int                    idx;
    uint32_t               time_elapsed_us;
    if (!context->dbg_info) {
        return;
    }
    adlak_os_printf("\n%s", __FUNCTION__);
#if 0
    for (idx = 0; idx < ADLAK_DBG_TIMESTAMP_MAX; idx++) {
        if (idx < info->start_idx) {
            adlak_os_printf("\ndbg[%d] %-20s timestamp %lld .", idx, info->identify_name[idx],
                            info->start[idx]);
        }
    }
#endif
    for (idx = 1; idx < ADLAK_DBG_TIMESTAMP_MAX; idx++) {
        if (idx < info->start_idx) {
            time_elapsed_us =
                (uint32_t)adlak_os_ktime_us_delta(info->start[idx], info->start[idx - 1]);

            adlak_os_printf("\ndbg[%d] %-30s -> %-30s used %d us.", idx,
                            info->identify_name[idx - 1], info->identify_name[idx],
                            time_elapsed_us);
        }
    }
#endif
}

void adlak_dbg_dump_module_read_data(struct adlak_context *context) {
#ifdef CONFIG_ADLAK_DEBUG_INNNER_MODULE

    struct adlak_device *padlak      = context->padlak;
    uint32_t *           module_data = (uint32_t *)context->dbg_info->vaddr_module_data;
    uint32_t             module_reg_max;
    struct io_region *   region = padlak->hw_res.preg;
    uint32_t             module, offset;

    uint32_t idx;
    adlak_os_printf("%s", __func__);
    adlak_write32(region, REG_ADLAK_DBG_EN, 1);  // debug enable
    idx = 0;
    for (module = 0; module < ADLAK_MODULE_COUNT; module++) {
        module_reg_max = ADLAK_MODULE_DATA_MAX;
        adlak_write32(region, REG_ADLAK_DBG_SEL, module);
        for (offset = 0; offset < module_reg_max;) {
            adlak_write32(region, REG_ADLAK_DBG_SUB_SEL, offset);
            module_data[idx] = adlak_read32(region, REG_ADLAK_DBG_DAT);
            offset++;
            idx++;
        }
    }

    adlak_write32(region, REG_ADLAK_DBG_EN, 0);  // debug disable
#endif
}

static void adlak_dbg_dump_module_get_module_name(uint32_t module, char *module_name, size_t size) {
    switch (module) {
        case 0:
            adlak_os_snprintf(module_name, size, "%s", "pwx");
            break;
        case 1:
            adlak_os_snprintf(module_name, size, "%s", "pwe");
            break;
        case 2:
            adlak_os_snprintf(module_name, size, "%s", "px");
            break;
        case 3:
            adlak_os_snprintf(module_name, size, "%s", "dmdw");
            break;
        case 4:
            adlak_os_snprintf(module_name, size, "%s", "dmdf");
            break;
        case 5:
            adlak_os_snprintf(module_name, size, "%s", "dw");
            break;
        case 6:
            adlak_os_snprintf(module_name, size, "%s", "pe");
            break;
        case 7:
            adlak_os_snprintf(module_name, size, "%s", "dmcw");
            break;
        case 8:
            adlak_os_snprintf(module_name, size, "%s", "dmcf");
            break;
        case 9:
            adlak_os_snprintf(module_name, size, "%s", "mc");
            break;
        case 10:
            adlak_os_snprintf(module_name, size, "%s", "rs");
            break;
        case 11:
            adlak_os_snprintf(module_name, size, "%s", "ps");
            break;
        case 12:
            adlak_os_snprintf(module_name, size, "%s", "ab");
            break;
        case 13:
            adlak_os_snprintf(module_name, size, "%s", "smmu");
            break;
    }
}

void adlak_dbg_dump_module_dump_data(struct adlak_context *context) {
#ifdef CONFIG_ADLAK_DEBUG_INNNER_MODULE

    uint32_t *module_data = (uint32_t *)context->dbg_info->vaddr_module_data;
    uint32_t  module, offset;
    char      module_name[32] = {0};

    uint32_t idx;
    adlak_os_printf("%s", __func__);
    idx    = 0;
    module = 0;
    for (idx = 0; idx < ADLAK_MODULE_COUNT * ADLAK_MODULE_DATA_MAX;) {
        if (0 == (idx % ADLAK_MODULE_DATA_MAX)) {
            adlak_os_msleep(100);
            adlak_dbg_dump_module_get_module_name(module, module_name, sizeof(module_name));
            adlak_os_printf("module:%s\n", module_name);
            offset = 0;
            module += 1;
        }
        adlak_os_printf("subsel:0x%08X\t0x%08X\n", (uint32_t)(offset), module_data[idx]);

        offset++;
        idx++;
    }
#endif
}
