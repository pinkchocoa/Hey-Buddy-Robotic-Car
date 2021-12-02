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
#define ONLED2 '1'
#define OFFLED2 '2'
#define ONREDLED '3'
#define OFFREDLED '4'

//raspberry follow
#define FOLLOWMID 'm'
#define FOLLOWLEFT 'l'
#define FOLLOWRIGHT 'r'



int main(void)
{
    /* Halting the watchdog */
    MAP_WDT_A_holdTimer();

    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0); // Configure P2.1 as output - LED2 - RED
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2); // Configure P2.2 as output - LED2 - BLUE
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    //moveM = moveL = moveR = false;

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
//        PCM_gotoLPM3InterruptSafe();
    }
}

/* Port1 ISR */
void PORT1_IRQHandler(void)
{
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    if (status & GPIO_PIN1) //S1 interrupt progressively step up the duty cycle of the PWM on a button press
    {
//        pwmConfig1.dutyCycle = (pwmConfig1.dutyCycle == 9000) ? 1000 : pwmConfig1.dutyCycle + 1000;
//        pwmConfig2.dutyCycle = (pwmConfig2.dutyCycle == 9000) ? 1000 : pwmConfig2.dutyCycle + 1000;

        // checking for boolean, it is to check and break out of the loop in the movement.h
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
    unsigned char buffer[15];

    msg = UART_receiveData(EUSCI_A0_BASE);

    //indicate that something was received
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);

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
        case ONLED2:
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
            break;
        case OFFLED2:
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
            break;
        case ONREDLED:
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
            break;
        case OFFREDLED:
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
            break;
        case FOLLOWMID:
            dist = getHCSR04DistanceFront();
            if ((dist > MIN_DISTANCE)){
                startMoving();
                //uPrintf("m\n\r");
            }
            else{
                zeroPWN();
            }
//            else{
//                snprintf((char*)buffer, 10, "%f", dist);
//                buffer[10] = '\\'; buffer[11]='n';buffer[12]='\\';buffer[13]='r';
//                uPrintf(buffer);
//            }
            break;
        case FOLLOWLEFT:
            dist = getHCSR04DistanceLeft();
            if ((dist > LR_MIN_DISTANCE)){
                rotateCarLeft();
                //uPrintf("l\n\r");
            }
            else{
                zeroPWN();
            }
//            else{
//                snprintf((char*)buffer, 10, "%f", dist);
//                buffer[10] = '\\'; buffer[11]='n';buffer[12]='\\';buffer[13]='r';
//                uPrintf(buffer);
//            }
            break;
        case FOLLOWRIGHT:
            dist = getHCSR04DistanceRight();
            if ((dist > LR_MIN_DISTANCE)){
                rotateCarRight();
                //uPrintf("r\n\r");
            }
            else{
                zeroPWN();
            }
//            else{
//                snprintf((char*)buffer, 10, "%f", dist);
//                buffer[10] = '\\'; buffer[11]='n';buffer[12]='\\';buffer[13]='r';
//                uPrintf(buffer);
//            }
            break;
        default:
            UART_transmitData(EUSCI_A0_BASE, msg);
    }
}

