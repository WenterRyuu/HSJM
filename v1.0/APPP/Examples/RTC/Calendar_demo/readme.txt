/*!
    \file    readme.txt
    \brief   description of calendar demo

    \version 2023-06-20, V1.1.0, firmware for GD32A50x
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

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
  This demo is based on the GD32A503V-EVAL-V1.1 board, it explains how to configure the 
RTC module. In this demo, RTC peripheral is configured to display calendar.
  Comment RTC_SRC_LXTAL_SELECT macro in rtc.h: select IRC40K as RTC clock source. 
  Uncomment RTC_SRC_LXTAL_SELECT macro in rtc.h: select LXTAL of bypass mode as RTC clock source, and
connect 32i pin to external 32,768HZ square wave signal.
  The HyperTerminal should be connected to the evaluation board via USART0.
  After start-up, get key value from BKP data register or bit RTCSRC of backup domain 
control register.  If the value is incorrect, the program will ask to set the time on 
the HyperTerminal. If the key value is correct, by the HyperTerminal set time,the 
calendar will be displayed on the HyperTerminal. 
  When uncomment RTCCLOCKOUTPUT_ENABLE, the PC13 pin will output RTC clock source divide 64.
  The RTC module is in the Backup Domain, and is not reset by the system reset.
