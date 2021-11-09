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
 *          --|RST         P1.1  |<--Toggle Switch - increase speed
 *            |            P1.4  |<--Toggle Switch - turn the car / stop the car
 *            |                  |
 *            |            P2.4  |--> Output PWM
 *            |            P2.5  |--> Output PWN
 *            |                  |
 *
 *******************************************************************************/
/* DriverLib Includes */

#include "movement.h"

int main(void)
{
    /* Halting the watchdog */
    MAP_WDT_A_holdTimer();

    setMotorPorts();
    startMoving();
    setS1S2Interrupt();
    generatePWN();

    while (1)
    {
        PCM_gotoLPM0();
    }
}

/* Port1 ISR */
void PORT1_IRQHandler(void)
{
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    if (status & GPIO_PIN1) //S1 interrupt progressively step up the duty cycle of the PWM on a button press
    {
        pwmConfig1.dutyCycle = (pwmConfig1.dutyCycle == 9000)? 1000 : pwmConfig1.dutyCycle + 1000;
        pwmConfig2.dutyCycle = (pwmConfig2.dutyCycle == 9000)? 1000 : pwmConfig2.dutyCycle + 1000;
    }

    if (status & GPIO_PIN4) //S2
    {
        rotating = rotating? zeroPWN() : rotateCarLeft();
    }
    generatePWN();
}
