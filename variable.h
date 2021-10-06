#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

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
}

