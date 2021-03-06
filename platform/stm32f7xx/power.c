/*
 * Copyright (c) 2016 Travis Geiselbrecht
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <debug.h>
#include <err.h>
#include <compiler.h>
#include <stdio.h>
#include <platform.h>
#include <platform/debug.h>
#include <arch/ops.h>
#include <arch/arm/cm.h>

void platform_halt(platform_halt_action suggested_action,
                          platform_halt_reason reason)
{
#if ENABLE_PANIC_SHELL
    if (reason == HALT_REASON_SW_PANIC) {
        dprintf(ALWAYS, "CRASH: starting debug shell... (reason = %d)\n", reason);
        arch_disable_ints();
        panic_shell_start();
    }
#endif  // ENABLE_PANIC_SHELL

    switch (suggested_action) {
        default:
        case HALT_ACTION_SHUTDOWN:
        case HALT_ACTION_HALT:
            dprintf(ALWAYS, "HALT: spinning forever... (reason = %d)\n", reason);
            arch_disable_ints();
            for(;;)
                arch_idle();
            break;
        case HALT_ACTION_REBOOT:
            dprintf(INFO, "REBOOT\n");
            arch_disable_ints();
            for (;;) {
                NVIC_SystemReset();
            }
            break;
    }

    dprintf(ALWAYS, "HALT: spinning forever... (reason = %d)\n", reason);
    arch_disable_ints();
    for(;;);
}

