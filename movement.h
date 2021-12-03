#pragma once
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "ultrasonic.h"

int detectleft = 0;
int detectright = 0;
float ratio = 1;

//store current duty cycle
int currDutyCycle1 = 0; //store left wheel duty cycle
int currDutyCycle2 = 0; //store right wheel duty cycle

/* Timer_A PWM Configuration Parameter */
//this configs for timer A register 1
//this is tied to P2.4
Timer_A_PWMConfig pwmConfig1 =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_24,
        10000,
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_RESET_SET,
        1000
};
//this configs for timer A register 1
//this is tied to P2.5
Timer_A_PWMConfig pwmConfig2 =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_24,
        10000,
        TIMER_A_CAPTURECOMPARE_REGISTER_2,
        TIMER_A_OUTPUTMODE_RESET_SET,
        1000
};


void setOutputOnMotor(){
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
}

void setMotorPorts(){
    //motor A
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN5);
    
    //motor B
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN2);
    
    //ENA
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
    
    //ENB
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);

    setOutputOnMotor();
}

//balance pwm
void setWheelInterupt(){
    //declare output of encoder(inputted to msp)
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P6, GPIO_PIN4);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN5);

    /* Enabling interrupts and starting the watchdog timer */
    GPIO_clearInterruptFlag(GPIO_PORT_P6, GPIO_PIN4);
    GPIO_enableInterrupt(GPIO_PORT_P6, GPIO_PIN4);
    GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN5);
    GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN5);

    Interrupt_enableInterrupt(INT_PORT5);
    Interrupt_enableInterrupt(INT_PORT6);

    Interrupt_enableSleepOnIsrExit();
    Interrupt_enableMaster();

}

void setS1S2Interrupt(){
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
    
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);

    /* Enabling interrupts and starting the watchdog timer */
    Interrupt_enableInterrupt(INT_PORT1);
    Interrupt_enableSleepOnIsrExit();
    Interrupt_enableMaster();
}



void generatePWN(){
    /* Configuring Timer_A to have a period of approximately 80ms and an initial duty cycle of 10% of that (1000 ticks)  */
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig1);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
}

void changeDirection(){
    GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN2);

    pwmConfig1.dutyCycle = pwmConfig2.dutyCycle = 3000;
    generatePWN();
}

void zeroPWN(){
    pwmConfig1.dutyCycle = pwmConfig2.dutyCycle = 0;
    generatePWN();
}

void startMoving(){
    if (getHCSR04DistanceFront() > MIN_DISTANCE){
        pwmConfig1.dutyCycle = pwmConfig2.dutyCycle = 5000;
        generatePWN();
    }
    else zeroPWN();
}


void rotateCarLeft(){
    if (getHCSR04DistanceLeft() > LR_MIN_DISTANCE){
        pwmConfig1.dutyCycle = 6000;
        pwmConfig2.dutyCycle = 4000;
        generatePWN();
    }
    else zeroPWN();
}

void rotateCarRight(){
    if (getHCSR04DistanceRight() > LR_MIN_DISTANCE){
        pwmConfig1.dutyCycle = 4000;
        pwmConfig2.dutyCycle = 6000;
        generatePWN();
    }
    else zeroPWN();
}


void PORT6_IRQHandler(void)
{
    //FOR RIGHT SIDE SLAVE
    uint32_t status;
    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P6);
    detectleft++;
    if(detectleft !=0 && detectright != 0 ){
        if(detectleft == 20){
            if (pwmConfig1.dutyCycle > 0 && pwmConfig2.dutyCycle > 0){
                if (pwmConfig1.dutyCycle == pwmConfig2.dutyCycle &&
                        getHCSR04DistanceFront() <= MIN_DISTANCE) {
                    zeroPWN();
                }
                else if (pwmConfig1.dutyCycle > pwmConfig2.dutyCycle &&
                        getHCSR04DistanceLeft() <= LR_MIN_DISTANCE) {
                    zeroPWN();
                }
                else if (pwmConfig1.dutyCycle < pwmConfig2.dutyCycle &&
                        getHCSR04DistanceRight() <= LR_MIN_DISTANCE) {
                    zeroPWN();
                }
            }
        }
        if(detectleft == 40){
            ratio = detectleft/detectright;
            Delay(3);
            pwmConfig1.dutyCycle = pwmConfig1.dutyCycle*ratio;
            generatePWN();
            detectleft=detectright=0;
        }
    }
    GPIO_clearInterruptFlag(GPIO_PORT_P6, status);
}

void PORT5_IRQHandler(void)
{
    //FOR LEFTSIDE SLAVE ENCODER
    uint32_t status;
    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    detectright++;
    if(detectleft !=0 && detectright != 0 ){
        if(detectright == 20){
            if (pwmConfig1.dutyCycle > 0 && pwmConfig2.dutyCycle > 0){
                if (pwmConfig1.dutyCycle == pwmConfig2.dutyCycle &&
                        getHCSR04DistanceFront() <= MIN_DISTANCE) {
                    zeroPWN();
                }
                else if (pwmConfig1.dutyCycle > pwmConfig2.dutyCycle &&
                        getHCSR04DistanceLeft() <= LR_MIN_DISTANCE) {
                    zeroPWN();
                }
                else if (pwmConfig1.dutyCycle < pwmConfig2.dutyCycle &&
                        getHCSR04DistanceRight() <= LR_MIN_DISTANCE) {
                    zeroPWN();
                }
            }
        }
        if(detectright == 40){
            ratio = detectright/detectleft;
            Delay(3);
            pwmConfig2.dutyCycle = pwmConfig2.dutyCycle*ratio;
            generatePWN();
            detectleft=detectright=0;
        }
    }

    GPIO_clearInterruptFlag(GPIO_PORT_P5, status);
}


