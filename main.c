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
 *          --|RST         P1.1  |<--Toggle Switch - change direction
 *            |                  |
 *            |            P1.4  |<--Toggle Switch - stop car
 *            |                  |
 *            |                  |
 *            |                  |
 *
 *******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "movement.h"
#include "serial.h"
#include <stdint.h>
#include <stdbool.h>

//define UART commands
#define FORWARD 'w'
#define LEFT 'a'
#define RIGHT 'd'
#define STOP 's'

#define ONRED '1'
#define OFFRED '2'
#define ONGREEN '3'
#define OFFGREEN '4'
#define ONBLUE '5'
#define OFFBLUE '6'
#define ONLED1 '7'
#define OFFLED1 '8'


int main(void)
{
    /* Halting the watchdog */
    MAP_WDT_A_holdTimer();

    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2); // Configure P2 LED
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0); // Configure P1 LED

    //off all LEDs first
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

    //initialization
    initUltraSensors();
    setWheelInterupt();
    setMotorPorts();
    setS1S2Interrupt();
    initUART();

    uPrintf("Going to Sleep\n\r");

    //low power mode and wait for interrupt
    while (1)
    {
        PCM_gotoLPM3InterruptSafe();
    }
}

/* Port1 ISR */
void PORT1_IRQHandler(void)
{
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    if (status & GPIO_PIN1) //S1 interrupt progressively step up the duty cycle of the PWM on a button press
    {
        changeDirection();
    }
    else if (status & GPIO_PIN4) //S2 interrupt to stop car
    {
        zeroPWN();
    }
}

//receive commands from raspberry pi through UART serial communication
void EUSCIA0_IRQHandler(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
    MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, status);
    unsigned char msg = 0;

    //receive data from raspberry pi
    msg = UART_receiveData(EUSCI_A0_BASE);

    //execute actions for the car
    switch (msg)
    {
        case FORWARD: // move the car forward
            startMoving();
            break;
        case LEFT: //turn the car left
            rotateCarLeft();
            break;
        case RIGHT:// turn the car right
            rotateCarRight();
            break;
        case STOP: // stop the car
            zeroPWN();
            break;
        case ONRED: //turn on red for led 2
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
            break;
        case OFFRED: //off red for led 2
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
            break;
        case ONGREEN: //turn on green for led 2
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
            break;
        case OFFGREEN: //turn off green for led 2
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
            break;
        case ONBLUE://turn on blue for led 2
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
            break;
        case OFFBLUE://turn off blue for led 2
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
            break;
        case ONLED1: //turn on red for led 1
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
            break;
        case OFFLED1://turn off red for led 1
            GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
            break;
        default: //pass data back to raspberry pi
            UART_transmitData(EUSCI_A0_BASE, msg);
    }
}

