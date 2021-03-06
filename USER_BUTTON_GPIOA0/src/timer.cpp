/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "timer.h"
#include "cortexm/exception-handlers.h"

// ----------------------------------------------------------------------------

#if defined(USE_HAL_DRIVER)
extern "C" void HAL_IncTick(void);
#endif

// ----------------------------------------------------------------------------

volatile timer::ticks_t timer::ms_delayCount;

// ----------------------------------------------------------------------------
/*
void
timer::start(void)
{
  // Use SysTick as reference for the delay loops.
  SysTick_Config(SystemCoreClock / FREQUENCY_HZ);
}


void
timer::sleep(ticks_t ticks)
{
  ms_delayCount = ticks;

  // Busy wait until the SysTick decrements the counter to zero.
  while (ms_delayCount != 0u)
    ;
}
*/
void
timer::tick(void)
{
  // Decrement to zero the counter used by the delay routine.
  if (ms_delayCount != 0u)
    {
      --ms_delayCount;
    }
}

// ----- SysTick_Handler() ----------------------------------------------------
/*
extern "C" void
SysTick_Handler(void)
{
#if defined(USE_HAL_DRIVER)
  HAL_IncTick();
#endif
  timer::tick();
}
*/
// ----------------------------------------------------------------------------
