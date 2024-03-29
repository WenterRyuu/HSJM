/*!
    \file    readme.txt
    \brief   description of the TIMERs cascade synchro demo for gd32a50x

    \version 2023-06-20, V1.1.0, firmware for GD32A50x
*/

/*
    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

  This demo is based on the GD32A503V-EVAL-V1.1 board, it shows how to synchronize 
TIMER peripherals in cascade mode.

  In this example three timers are used:
  1/TIMER1 is configured as master timer:
  - PWM mode 0 is used
  - The TIMER1 update event is used as trigger output

  2/TIMER7 is slave for TIMER1 and master for TIMER0:
  - PWM mode 0 is used
  - The ITI0(TIMER1) is used as input trigger 
  - External clock mode is used,the counter counts on the rising edges of the selected
    trigger.
  - The TIMER7 update event is used as trigger output.

  3/TIMER0 is slave for TIMER7:
  - PWM mode 0 is used
  - The ITI0(TIMER7) is used as input trigger
  - External clock mode is used,the counter counts on the rising edges of the selected
  trigger.

  The TIMERxCLK is fixed to 100 MHz, the TIMER1 counter clock is: 100MHz/5000= 20 KHz.

  The master timer TIMER1 is running at TIMER1 frequency :
  TIMER1 frequency = (TIMER1 counter clock)/ (TIMER1 period + 1) = 5 Hz, and the duty
cycle = TIMER1_CH0CV/(TIMER1_CAR + 1) = 50%

  The TIMER7 is running:
  - At (TIMER1 frequency)/ (TIMER7 period + 1) = 2.5 Hz and a duty cycle equal to 
TIMER7_CH0CV/(TIMER7_CAR + 1) = 50%

  The TIMER0 is running:
  - At (TIMER7 frequency)/ (TIMER0 period + 1) = 1.25 Hz and a duty cycle equal to
TIMER0_CH0CV/(TIMER0_CAR + 1) = 50%
