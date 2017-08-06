/*
 * Copyright (c) 2013-2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "uvisor-lib/uvisor-lib.h"
#include "mbed.h"
#include "main-hw.h"

/* Create ACLs for main box. */
MAIN_ACL(g_main_acl);

/* Enable uVisor. */
UVISOR_SET_MODE_ACL(UVISOR_ENABLED, g_main_acl);
UVISOR_SET_PAGE_HEAP(8 * 1024, 5);

/* Targets with an ARMv7-M MPU needs this space adjustment to prevent a runtime
 * memory overflow error. The code below has been output directly by uVisor. */
#if defined(TARGET_EFM32GG_STK3700) || defined(TARGET_DISCO_F429ZI)
uint8_t __attribute__((section(".keep.uvisor.bss.boxes"), aligned(32))) __boxes_overhead[8064];
#endif

/* Configure box 0 as the debug box. */

static uint32_t get_version(void) {
    return 0;
}

static void halt_error(THaltError reason, const THaltInfo *halt_info) {
    const char *exception_stack[] = {"R0", "R1", "R2", "R3", "R12", "LR", "PC", "xPSR"};

    printf("Failed with an error code: 0x%08x\r\n\r\n", reason);

	if (halt_info == NULL) {
		return;
	}

    if ((halt_info->valid_data & HALT_INFO_STACK_FRAME)) {
        printf("Exception stack frame:\r\n");
        for (int i = sizeof(exception_stack) / sizeof(exception_stack[0]) - 1; i >= 0; i--)
        {
            printf("SP[%02d]: 0x%08x | %s\r\n", i, ((uint32_t *)&halt_info->stack_frame)[i], exception_stack[i]);
        }
        printf("\r\n");
    }

    if ((halt_info->valid_data & HALT_INFO_REGISTERS)) {
        printf("Registers after fault:\r\n");
        printf("LR:      0x%08x\r\n", halt_info->lr);
        printf("IPSR:    0x%08x\r\n", halt_info->ipsr);
        printf("CONTROL: 0x%08x\r\n", halt_info->control);
        printf("\r\n");

        printf("Fault registers:\r\n");
        printf("CFSR:  0x%08x\r\n", halt_info->cfsr);
        printf("HFSR:  0x%08x\r\n", halt_info->hfsr);
        printf("DFSR:  0x%08x\r\n", halt_info->dfsr);
        printf("AFSR:  0x%08x\r\n", halt_info->afsr);
        printf("MMFAR: 0x%08x\r\n", halt_info->mmfar);
        printf("BFAR:  0x%08x\r\n", halt_info->bfar);
        printf("\r\n");
    }
}

/* Debug box driver -- Version 0 */
static TUvisorDebugDriver const g_driver = {
    get_version,
    halt_error
};

#define BAD_BAD_ADDR (*((volatile unsigned long *) (0xFFFFFFFF)))

int main(void)
{
    /* Register the debug box with uVisor. */
    uvisor_debug_init(&g_driver);
    while (1){
        BAD_BAD_ADDR = 13;
    }

    return 0;
}
