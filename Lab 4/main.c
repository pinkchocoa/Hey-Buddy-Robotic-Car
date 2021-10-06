/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/*******************************************************************************
 * MSP432 Timer_A - Variable PWM
 *
 * Description: In this example, the Timer_A module is used to create a precision
 * PWM with an adjustable duty cycle. The PWM initial period is 80 ms and is
 * output on P2.4. The initial duty cycle of the PWM is 10%, however when the
 * button is pressed on P1.1 the duty cycle is sequentially increased by 10%.
 * Once the duty cycle reaches 90%, the duty cycle is reset to 10% on the
 * following button press.
 * Ultrasonic sensor
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P1.1  |<--Toggle Switch
 *            |                  |
 *            |                  |
 *            |            P2.4  |--> Output PWM
 *            |                  |
 *            |                  |
 *
 *******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#define MIN_DIST 15.0f
#define TICKPERIOD 1000

uint32_t SR04IntTimes;

static void Delay (uint32_t loop){
    volatile uint32_t i;
    for (i = 0; i < loop; i++);
}
void Initialise_HCSR04(void){
//    Timer A upmode conf
    //MCLK is used as clock source for the CPU.
//    SMCLK is usually a high frequency clock and it is used for peripheral modules (e.g. Timers, serial communication modules, ...)
//    ACLK is usually a 32kHz crystal clock. It is used for peripheral modules that require a low-frequency clock (e.g. real-time-clock,  ...)
    const Timer_A_UpModeConfig upConfig =
    {
         TIMER_A_CLOCKSOURCE_SMCLK, //set source
         TIMER_A_CLOCKSOURCE_DIVIDER_3,// SMCLK/3 = 1 MHz = 1 microsec = 1 microsec * 10000 = 1ms
         TICKPERIOD,
         TIMER_A_TAIE_INTERRUPT_DISABLE,//DISABLE TIMER A INTERUPT
         TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE, // ENAABLE DEFAULT IS DISABLE
         TIMER_A_DO_CLEAR //CLEAR TIMER COUNTER

    };

    int a = CS_getSMCLK(); //GET FREQ OF CLOCK SOURCE default 3MHz

    //CONF 3.6 AS OUTPUT LOW SO IT WILL NOT ACCIDENTALLY TRIGGER US SENSOR
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN6);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN6);
    //3.7 INPUT LOW
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN7);

//    CONF TIMER A0
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);

//    enable interrupt and start timer
    Interrupt_enableInterrupt(INT_TA0_0);
//    clear timer
    Timer_A_clearTimer(TIMER_A0_BASE);
}

//EVERY 1000 TICK = 1 ISR interuppt
void TAO_0_IRQHandler(void){
    //incre global var
    SR04IntTimes++;
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
}

static uint32_t getHCSR04Time(void){
    uint32_t pulsetime = 0;
//    1 tick = 1us, 1000 TICK = 1000 * 1 us = 1ms
    pulsetime = SR04IntTimes * TICKPERIOD;

    //NUM OF TICK BEFORE INT HAPPEN
//    WHEN STOP TIMER, VAL STILL IN COUNTER
    pulsetime += Timer_A_getCounterValue(TIMER_A0_BASE);

    Timer_A_clearTimer(TIMER_A0_BASE);

    Delay(3000); // 30ms

    return pulsetime;

}

float getHCSR04Distance(void){
    uint32_t pulseDuration = 0;
    float calDist = 0;

//   Trigger generate pulse p3.6 at least 10us
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN6);
    Delay(30); //30 = 3ms
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN6);

//  Echo wait for positive edge
    while(GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN7) == 0);
//    printf("%f\n",GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN7));


    //start time when echo go positive cause it will exit the while loop
    //RESET GLOBAL VAR , CLEAR TIMER THEN START COUNT
    SR04IntTimes = 0;
    Timer_A_clearTimer(TIMER_A0_BASE);
    Timer_A_startCounter(TIMER_A0_BASE,TIMER_A_UP_MODE);

    //Echo wait for negative edge
    while(GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN7) == 1);

//    Stops when it goes low
    Timer_A_stopTimer(TIMER_A0_BASE);

//    obtain pulse width in microsec
    pulseDuration = getHCSR04Time();

    //cal dist in cm
    calDist = (float)pulseDuration / 58.0f;

    printf("%f\n",calDist);

    return calDist;

}

uint32_t main(void){
    Initialise_HCSR04();

//    conf led 1.0 to low
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    while(1){
        Delay(3000);
        if(getHCSR04Distance() < MIN_DIST){
              GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
        }
        else{

              GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        }
    }
}





