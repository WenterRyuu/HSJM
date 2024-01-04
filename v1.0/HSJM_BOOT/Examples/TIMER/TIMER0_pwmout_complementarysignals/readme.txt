/*!
    \file    readme.txt
    \brief   description of the TIMER0 pwmout complementary signals demo for gd32a50x

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

  This demo is based on the GD32A503V-EVAL-V1.1 board, it shows how to configure the
TIMER0 peripheral to generate four complementary TIMER0 signals.

  TIMER0CLK is fixed to systemcoreclock, the TIMER0 prescaler is equal to 5000 so the
TIMER0 counter clock used is 20KHz.

  The four duty cycles are computed as the following description:
    the channel 0 duty cycle is set to 20%, so multi mode channel 0 is set to 80%.
    the channel 1 duty cycle is set to 40%, so multi mode channel 1 is set to 60%.
    the channel 2 duty cycle is set to 60%, so multi mode channel 2 is set to 40%.
    the channel 3 duty cycle is set to 80%, so multi mode channel 3 is set to 20%.

  The CH0/MCH0 and CH1/MCH1 are configured in PWM mode 0, and the CH2/MCH2 and 
CH3/MCH3 are in composite PWM mode(with PWM mode 1).

  Connect the TIMER0 pins to an oscilloscope to monitor the different waveforms:
    - TIMER0_CH0  pin (PC8)
    - TIMER0_MCH0 pin (PC7)
    - TIMER0_CH1  pin (PA4)
    - TIMER0_MCH1 pin (PA3)
    - TIMER0_CH2  pin (PA2)
    - TIMER0_MCH2 pin (PA1)
    - TIMER0_CH3  pin (PD12)
    - TIMER0_MCH3 pin (PD11)
