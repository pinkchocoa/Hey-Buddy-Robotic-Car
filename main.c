/*******************************************************************************
 * MSP432 Timer_A - Variable PWM
 *
 * Description: In this example, the Timer_A module is used to create a precision
 * PWM with an adjustable duty cycle. The PWM initial period is 80 ms and is
 * output on P2.4. The initial duty cycle of the PWM is 10%, however when the
 * button is pressed on P1.1 the duty cycle is sequentially increased by 10%.
 * Once the duty cycle reaches 90%, the duty cycle is reset to 10% on the
 * following button press.
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P1.1  |<--Toggle Switch
 *            |            P1.4  |<--Toggle Switch
 *            |                  |
 *            |            P2.4  |--> Output PWM
 *            |            P2.5  |--> Output PWN
 *            |                  |
 *
 *******************************************************************************/
/* DriverLib Includes */

#include "variable.h"

int main(void)
{
    /* Halting the watchdog */
    MAP_WDT_A_holdTimer();

    setMotorPorts();

    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);

    setS1S2Interrupt();
    generatePWN();

    /* Enabling interrupts and starting the watchdog timer */
        Interrupt_enableInterrupt(INT_PORT1);
        Interrupt_enableSleepOnIsrExit();
        Interrupt_enableMaster();

    while (1)
    {
        PCM_gotoLPM0();
    }
}

/* Port1 ISR - This ISR will progressively step up the duty cycle of the PWM on a button press */
void PORT1_IRQHandler(void)
{
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    if (status & GPIO_PIN1) //S1 interrupt
    {
        if(pwmConfig1.dutyCycle == 9000)
            pwmConfig1.dutyCycle = 1000;
        else
            pwmConfig1.dutyCycle += 1000;

        if(pwmConfig2.dutyCycle == 9000)
            pwmConfig2.dutyCycle = 1000;
        else
            pwmConfig2.dutyCycle += 1000;

        generatePWN();
    }
    if (status & GPIO_PIN4) //S2 interrupt
    {
        GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN4);
        GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN5);
        GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN0);
        GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN2);
    }
}
