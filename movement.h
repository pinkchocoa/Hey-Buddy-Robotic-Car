#pragma once
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
<<<<<<< Updated upstream
int detectleft = 0;
int detectright = 0;
float ratio = 1;
=======
#include <stdio.h>

float detectleft = 0.0;
float detectright = 0.0;
float ratio =1.0;

//store current duty cycle
int currDutyCycle1 = 0; //store left wheel duty cycle
int currDutyCycle2 = 0; //store right wheel duty cycle

>>>>>>> Stashed changes
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

void BalancePWM(){
    while (currDutyCycle1 != currDutyCycle2){
        if (currDutyCycle1>currDutyCycle2){
            currDutyCycle2 += 1000;
        }
        else{
            currDutyCycle2 -= 1000;
        }
    }
    generatePWN();
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
    pwmConfig1.dutyCycle = 5000;
    pwmConfig2.dutyCycle = 5000;

<<<<<<< Updated upstream
=======
    // Storing duty cycle to check pwm speed (for jin & josh PID)
    currDutyCycle1 = pwmConfig1.dutyCycle;
    currDutyCycle2 = pwmConfig2.dutyCycle;
    generatePWN();
    PORT6_IRQHandler();
    PORT5_IRQHandler();

    printf("PWM Left side: %d Right side: %d \n" ,currDutyCycle1,currDutyCycle2);
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
    pwmConfig2.dutyCycle = 1000;
    pwmConfig1.dutyCycle = 5000;
=======

    pwmConfig1.dutyCycle = 3000;
    pwmConfig2.dutyCycle = 6000;

    // Storing duty cycle to check pwm speed (for jin & josh PID)
    currDutyCycle1 = pwmConfig1.dutyCycle;
    currDutyCycle2 = pwmConfig2.dutyCycle;

    generatePWN();
    printf("PWM Left side: %d Right side: %d \n" ,currDutyCycle1,currDutyCycle2);
    BalancePWM();

    printf("PWM Left side: %d Right side: %d \n" ,currDutyCycle1,currDutyCycle2);
>>>>>>> Stashed changes
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
        if(detectleft == 40){
            ratio = detectleft/ detectright;
            printf("leftratio: %f\n",ratio);
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
        if(detectright == 40){
            ratio = detectright/detectleft;
            printf("rightratio: %f\n",ratio);
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

<<<<<<< Updated upstream
=======
// this function is to store the previous config1 & config2 duty cycle and run it.
void previousPWM(config1, config2){
    int previousDutyCycle1 = 0;
    int previousDutyCycle2 = 0;

    //store the last duty cycle
    previousDutyCycle1 = config1;
    previousDutyCycle2 = config2;

    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);

    pwmConfig1.dutyCycle =  previousDutyCycle1;
    pwmConfig2.dutyCycle =  previousDutyCycle2;

    // make a global variable & store the pwm
    generatePWN();

}


>>>>>>> Stashed changes
