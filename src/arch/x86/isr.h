/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * ****************************************************************************/
#ifndef __ISR_H_
#define __ISR_H_

#include <stdint.h>
#include "cpu.h"

void fault_handler(cpu_state_t cpu_state, stack_state_t stack, uint32_t intr_no, uint32_t error_no);
void interrupt_handler(cpu_state_t cpu_state, stack_state_t stack, uint32_t intr_no, uint32_t error_no);
void irq_handler(cpu_state_t cpu_state, stack_state_t stack, uint32_t intr_no, uint32_t error_no);

#endif  //	__ISR_H_
