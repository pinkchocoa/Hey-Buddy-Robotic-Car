#pragma once
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "ultrasonic.h"

/*these variables are used for the encoder
to sync up the speed between the two wheels*/
int detectleft = 0;
int detectright = 0;
float ratio = 1;

/* Timer_A PWM Configuration Parameter */
//this config for timer A register 1, p2.4
Timer_A_PWMConfig pwmConfig1 =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_24,
        10000,
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_RESET_SET,
        1000
};

//this config for timer A register 2, p2.5
Timer_A_PWMConfig pwmConfig2 =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_24,
        10000,
        TIMER_A_CAPTURECOMPARE_REGISTER_2,
        TIMER_A_OUTPUTMODE_RESET_SET,
        1000
};

//this function sets the output on motor for it to go forward
void setOutputOnMotor(){
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
}

//this function sets the needed ports as output pins for wheel movement
void setMotorPorts(){
    //motor A
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN5);
    
    //motor B
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN2);
    
    //ENA
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,
                                                GPIO_PIN4,
                                                GPIO_PRIMARY_MODULE_FUNCTION);
    
    //ENB
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,
                                                GPIO_PIN5,
                                                GPIO_PRIMARY_MODULE_FUNCTION);

    setOutputOnMotor();
}

//this function sets interrupt for port 5 and 6, for the wheel encoders
void setWheelInterupt(){
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P6, GPIO_PIN4);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN5);

    GPIO_clearInterruptFlag(GPIO_PORT_P6, GPIO_PIN4);
    GPIO_enableInterrupt(GPIO_PORT_P6, GPIO_PIN4);
    GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN5);
    GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN5);

    Interrupt_enableInterrupt(INT_PORT5);
    Interrupt_enableInterrupt(INT_PORT6);

    Interrupt_enableSleepOnIsrExit();
    Interrupt_enableMaster();
}

//this function sets interrupt for port 1, for S1 and S2
void setS1S2Interrupt(){
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
    
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);

    Interrupt_enableInterrupt(INT_PORT1);
    Interrupt_enableSleepOnIsrExit();
    Interrupt_enableMaster();
}

//this function generates PWM for timer A, register 1 and 2
void generatePWN(){
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig1);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
}

//this function toggles the wheels output to change its direction
void changeDirection(){
    GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN2);

    generatePWN();
}

//this function sets the PWM to 0 to stop the car
void zeroPWN(){
    pwmConfig1.dutyCycle = pwmConfig2.dutyCycle = 0;
    generatePWN();
}

//this function moves the car forward
void startMoving(){
    //check front ultrasonic to see if moving forward is possible
    if (getHCSR04DistanceFront() > MIN_DISTANCE){
        pwmConfig1.dutyCycle = pwmConfig2.dutyCycle = 5000;
        generatePWN();
    }
    else zeroPWN(); //no space to move in front, stop
}

//this function turns the car left
void rotateCarLeft(){
    //check left ultrasonic to see if turning left is possible
    if (getHCSR04DistanceLeft() > LR_MIN_DISTANCE){
        pwmConfig1.dutyCycle = 6000;
        pwmConfig2.dutyCycle = 4000;
        generatePWN();
    }
    else zeroPWN(); //no space to turn left, stop
}

//this function turns the car right
void rotateCarRight(){
    //check right ultrasonic to see if turning left is possible
    if (getHCSR04DistanceRight() > LR_MIN_DISTANCE){
        pwmConfig1.dutyCycle = 4000;
        pwmConfig2.dutyCycle = 6000;
        generatePWN();
    }
    else zeroPWN(); //no space to turn right, stop
}

//this function checks all three ultrasonic sensors
//dependent on the current state of the PWM duty cycles
void checkUltraSonic(){
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

//port 6 interrupt handler to sync the wheels speed
void PORT6_IRQHandler(void)
{
    //FOR RIGHT SIDE SLAVE
    uint32_t status;
    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P6);
    detectleft++;
    if(detectleft !=0 && detectright != 0 ){
        if(detectleft == 40){
            ratio = detectleft/detectright;
            checkUltraSonic();
            pwmConfig1.dutyCycle = pwmConfig1.dutyCycle*ratio;
            generatePWN();
            detectleft=detectright=0;
        }
    }
    GPIO_clearInterruptFlag(GPIO_PORT_P6, status);
}

//port 5 interrupt handler to sync the wheels speed
void PORT5_IRQHandler(void)
{
    //FOR LEFTSIDE SLAVE ENCODER
    uint32_t status;
    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    detectright++;
    if(detectleft !=0 && detectright != 0 ){
        if(detectright == 40){
            ratio = detectright/detectleft;
            checkUltraSonic();
            pwmConfig2.dutyCycle = pwmConfig2.dutyCycle*ratio;
            generatePWN();
            detectleft=detectright=0;
        }
    }
    GPIO_clearInterruptFlag(GPIO_PORT_P5, status);
}


