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


/* Declaring a function to be used as halt_error function for the debug box. */
static void example_halt_error(int reason) {
    printf("***** uVisor debug box example *****\n");
    printf("Tried to access address 0xFFFFFFFF which is not allowed!!!!\n");
	printf("Bye Bye Now!!!!!!\n");
}

/* Declare g_debug_driver and use example_halt_error() as its halt_error() function */
UVISOR_GENERATE_DEBUG_DRIVER(example_halt_error);


/* Create ACLs for main box. */
MAIN_ACL(g_main_acl);

/* Enable uVisor and register the public box to the debug driver */
UVISOR_SET_MODE_ACL_DBGBOX(UVISOR_ENABLED, g_main_acl);
UVISOR_SET_PAGE_HEAP(8 * 1024, 5);


/* Targets with an ARMv7-M MPU needs this space adjustment to prevent a runtime
 * memory overflow error. The code below has been output directly by uVisor. */
#if defined(TARGET_EFM32GG_STK3700) || defined(TARGET_DISCO_F429ZI)
uint8_t __attribute__((section(".keep.uvisor.bss.boxes"), aligned(32))) __boxes_overhead[8064];
#endif


#define BAD_BAD_ADDR (*((volatile unsigned long *) (0xFFFFFFFF)))

int main(void)
{
    printf("\r\n***** uVisor debug-fault example *****\r\n");

    while (1){
        BAD_BAD_ADDR = 13;
    }

    return 0;
}
