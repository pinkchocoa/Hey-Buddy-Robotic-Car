#pragma once
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
int detectleft = 0;
int detectright = 0;
float ratio = 1;
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
        3000
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
        3000
};

bool rotating = false;

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
    Interrupt_enableInterrupt(INT_PORT1);
    Interrupt_enableSleepOnIsrExit();
    Interrupt_enableMaster();

}

void generatePWN(){
    /* Configuring Timer_A to have a period of approximately 80ms and an initial duty cycle of 10% of that (1000 ticks)  */
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig1);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
}

void startMoving(){
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
    pwmConfig1.dutyCycle = pwmConfig2.dutyCycle = 5000;

}

void changeDirection(){
    GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN2);
}

bool rotateCarLeft(){
    pwmConfig1.dutyCycle = 1000;
    pwmConfig2.dutyCycle = 5000;
    return true;
}

bool rotateCarRight(){
    pwmConfig2.dutyCycle = 1000;
    pwmConfig1.dutyCycle = 5000;
    return true;
}

bool resetPWN(){
    pwmConfig1.dutyCycle = pwmConfig2.dutyCycle = 3000;
    return false;
}

bool zeroPWN(){
    pwmConfig1.dutyCycle = pwmConfig2.dutyCycle = 0;
    return false;
}

void PORT6_IRQHandler(void)
{
    //FOR RIGHT SIDE SLAVE
    uint32_t status;
    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P6);
    detectleft++;
    if(detectleft !=0 && detectright != 0 ){
        if(detectleft == 10){
            ratio = detectleft/detectright;
            pwmConfig1.dutyCycle = pwmConfig1.dutyCycle*ratio;
            Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
            Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig1);
            detectleft=0;
            detectright=0;
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
        if(detectright == 10){
            ratio = detectright/detectleft;
            pwmConfig2.dutyCycle = pwmConfig2.dutyCycle*ratio;
            Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
            Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig1);
            detectright=0;
            detectleft=0;
        }
    }
    GPIO_clearInterruptFlag(GPIO_PORT_P5, status);
}

void MoveLeft(void){
    pwmConfig1.dutyCycle = 3000;
    pwmConfig2.dutyCycle = 8000;
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig1);
    //Moveforward();
}

void MoveRight(void){
    pwmConfig1.dutyCycle = 8000;
    pwmConfig2.dutyCycle = 3000;
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig1);
}

