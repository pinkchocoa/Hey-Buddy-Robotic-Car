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
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "movement.h"
#include "serial.h"
#include "ultrasonic.h"
#include <stdint.h>
#include <stdbool.h>

#define FORWARD 'w'
#define LEFT 'a'
#define RIGHT 'd'
#define BACK 's'

#define ONRED '1'
#define OFFRED '2'
#define ONGREEN '3'
#define OFFGREEN '4'
#define ONBLUE '5'
#define OFFBLUE '6'
#define ONLED1 '7'
#define OFFLED1 '8'

//raspberry follow
#define FOLLOWMID 'm'
#define FOLLOWLEFT 'l'
#define FOLLOWRIGHT 'r'



int main(void)
{
    /* Halting the watchdog */
    MAP_WDT_A_holdTimer();

    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN0); // Configure P2 LED
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0); // Configure P1 LED

    //ultra sensors
    initUltraSensors();
    check=false; //condition for while loop
    //pwm
    setMotorPorts();
    //interrupt
    setS1S2Interrupt();
    //communicate with pi
    initUART();

    uPrintf("Going to Sleep\n\r");

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
        check=true;
        startUltraSensors();
    }
}

void EUSCIA0_IRQHandler(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
    MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, status);
    unsigned char msg = 0;
    float dist = 0.0f;
    //unsigned char buffer[15];

    msg = UART_receiveData(EUSCI_A0_BASE);

    //indicate that something was received
    //GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);

    switch (msg)
    {
        case FORWARD:
            uPrintf("w\n\r");
            break;
        case LEFT:
            uPrintf("a\n\r");
            break;
        case RIGHT:
            uPrintf("d\n\r");
            break;
        case BACK:
            //uPrintf("s\n\r");
            zeroPWN();
            break;
        case ONRED:
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
            break;
        case OFFRED:
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
            break;
        case ONGREEN:
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
            break;
        case OFFGREEN:
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
            break;
        case ONBLUE:
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
            break;
        case OFFBLUE:
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
            break;
        case ONLED1:
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
            break;
        case OFFLED1:
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN1);
            break;
        case FOLLOWMID:
            dist = getHCSR04DistanceFront();
            if ((dist > MIN_DISTANCE)){
                startMoving();
                //uPrintf("m\n\r");
            }
            else{ zeroPWN();}
//            else{
//                snprintf((char*)buffer, 10, "%f", dist);
//                buffer[10] = '\\'; buffer[11]='n';buffer[12]='\\';buffer[13]='r';
//                uPrintf(buffer);
//            }
            break;
        case FOLLOWLEFT:
            (getHCSR04DistanceLeft() > LR_MIN_DISTANCE)?rotateCarLeft():zeroPWN();
            break;
        case FOLLOWRIGHT:
            (getHCSR04DistanceRight() > LR_MIN_DISTANCE)?rotateCarRight():zeroPWN();
            else{ zeroPWN(); }
            break;
        default:
            UART_transmitData(EUSCI_A0_BASE, msg);
    }
}

