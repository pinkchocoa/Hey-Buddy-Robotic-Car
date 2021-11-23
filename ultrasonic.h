/*******************************************************************************
 * MSP432 Timer Interrupt
 *
 *
 *                MSP432P401
 *
 *               Ultrasonic left
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P5.2  |---> Trigger right
 *            |                  |
 *            |            P3.5  |<--- Echo left
 *            |                  |
 *            |                  |
 *
 *              Ultrasonic right
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P5.0  |---> Trigger Right
 *            |                  |
 *            |            P5.1  |<--- Echo left
 *            |                  |
 *            |                  |
 *
 *
 *             Ultrasonic Front
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P3.6  |---> Trigger Right
 *            |                  |
 *            |            P3.7  |<--- Echo left
 *            |                  |
 *            |                  |
 *
 *
 *
 ******************************************************************************/

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#include "movement.h"
#define MIN_DISTANCE    30.0f //30 cm
#define TICKPERIOD      1000

int SR04IntTimesRight;
int SR04IntTimesLeft;
int SR04IntTimesFront;

// -------------------------------------------------------------------------------------------------------------------
static void Delay(int loop)
{
    volatile int i;
    for (i = 0; i < loop; i++);
}

// ------------------------------------------------------Initialise all ultrasonic sensors -------------------------------------------------------------
void Initialise_HCSR04(void)
{
    /* Timer_A UpMode Configuration Parameter */
    const Timer_A_UpModeConfig upConfigRight = {
                 TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
                 TIMER_A_CLOCKSOURCE_DIVIDER_3,          // SMCLK/3 = 1MHz
                 TICKPERIOD,                             // 1000 tick period
                 TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
                 TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,    // Enable CCR0 interrupt
                 TIMER_A_DO_CLEAR                        // Clear value
    };

    const Timer_A_UpModeConfig upConfigLeft = {
                 TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
                 TIMER_A_CLOCKSOURCE_DIVIDER_3,          // SMCLK/3 = 1MHz
                 TICKPERIOD,                             // 1000 tick period
                 TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
                 TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,    // Enable CCR0 interrupt
                 TIMER_A_DO_CLEAR                        // Clear value
    };

    const Timer_A_UpModeConfig upConfigFront = {
                 TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
                 TIMER_A_CLOCKSOURCE_DIVIDER_3,          // SMCLK/3 = 1MHz
                 TICKPERIOD,                             // 1000 tick period
                 TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
                 TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,    // Enable CCR0 interrupt
                 TIMER_A_DO_CLEAR                        // Clear value
    };

    /* Configuring 5.2,3.6,5.0 as Output - trigger sensor*/
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN2);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN6);

    //prevent sensor from triggering
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN6);

    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN7);


    /* Configuring Timer_A0 for Up Mode */
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfigRight);
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfigLeft);
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfigFront);

    /* Enabling interrupts and starting the timer */
    Interrupt_enableInterrupt(INT_TA0_0);

    //Timer_A_stopTimer(TIMER_A0_BASE);
    Timer_A_clearTimer(TIMER_A0_BASE);
}

// ----------------------------------------------interruot handler---------------------------------------------------------------------
//1000 ticks occur = call isr
void TA0_0_IRQHandler(void)
{
    /* Increment global variable (count number of interrupt occurred) */
    SR04IntTimesRight++;
    SR04IntTimesLeft++;
    SR04IntTimesFront++;

    /* Clear interrupt flag */
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,
                                         TIMER_A_CAPTURECOMPARE_REGISTER_0);
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,
                                         TIMER_A_CAPTURECOMPARE_REGISTER_1);
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,
                                         TIMER_A_CAPTURECOMPARE_REGISTER_2);
}

// ---------------------------------------------------get distance for right sensor----------------------------------------------------------------
static int getHCSR04TimeRight(void)
{
    int pulsetime = 0;

    /* Number of times the interrupt occurred (1 interrupt = 1000 ticks)    */
    pulsetime = SR04IntTimesRight * TICKPERIOD;

    /* Number of ticks (between 1 to 999) before the interrupt could occur  */
    pulsetime += Timer_A_getCounterValue(TIMER_A0_BASE);

    /* Clear Timer */
    Timer_A_clearTimer(TIMER_A0_BASE);

    Delay(3000);

    return pulsetime;
}

float getHCSR04DistanceRight(void)
{
    int pulseduration = 0;
    float calculateddistance = 0;

    /* Generate 10us pulse at P5.2 */
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN0);
    Delay(30);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0);

    /* Wait for positive-edge */
    while (GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN1) == 0);

    /* Start Timer */
    SR04IntTimesRight = 0;
    Timer_A_clearTimer(TIMER_A0_BASE);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    /* Detects negative-edge */
    while (GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN1) == 1);

    /* Stop Timer */
    Timer_A_stopTimer(TIMER_A0_BASE);

    /* Obtain Pulse Width in microseconds */
    pulseduration = getHCSR04TimeRight();

    /* Calculating distance in cm */
    calculateddistance = (float) pulseduration / 58.0f;
//    printf("Right Ultrasonic Distance: %.2fcm\n", calculateddistance);

    return calculateddistance;
}

// --------------------------------------------------get distance for left sensor-----------------------------------------------------------------
static int getHCSR04TimeLeft(void)
{
    int pulsetime = 0;

    /* Number of times the interrupt occurred (1 interrupt = 1000 ticks)    */
    pulsetime = SR04IntTimesLeft * TICKPERIOD;

    /* Number of ticks (between 1 to 999) before the interrupt could occur  */
    pulsetime += Timer_A_getCounterValue(TIMER_A0_BASE);

    /* Clear Timer */
    Timer_A_clearTimer(TIMER_A0_BASE);

    Delay(3000);

    return pulsetime;
}
float getHCSR04DistanceLeft(void)
{
    int pulseduration = 0;
    float calculateddistance = 0;

    /* Generate 10us pulse at P5.0 */
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN2);
    Delay(30);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);

    /* Wait for positive-edge (continue until echo goes high = 1) */
    while (GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN5) == 0);


    /* Start Timer - echo goes high - reset global var and count number of ticks*/
    SR04IntTimesLeft = 0;
    Timer_A_clearTimer(TIMER_A0_BASE);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    /* Detects negative-edge */
    while (GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN5) == 1);

    /* Stop Timer - when echo goes low*/
    Timer_A_stopTimer(TIMER_A0_BASE);

    /* Obtain Pulse Width in microseconds */
    pulseduration = getHCSR04TimeLeft();

    /* Calculating distance in cm */
    calculateddistance = (float) pulseduration / 58.0f;
//    printf("Left Ultrasonic Distance: %.2fcm\n", calculateddistance);

    return calculateddistance;
}



// --------------------------------------------------get distance for front sensor-----------------------------------------------------------------
static int getHCSR04TimeFront(void)
{
    int pulsetime = 0;

    /* Number of times the interrupt occurred (1 interrupt = 1000 ticks)    */
    pulsetime = SR04IntTimesFront * TICKPERIOD;

    /* Number of ticks (between 1 to 999) before the interrupt could occur  */
    //val still in counter count remaining pulse
    pulsetime += Timer_A_getCounterValue(TIMER_A0_BASE);

    /* Clear Timer */
    Timer_A_clearTimer(TIMER_A0_BASE);

    Delay(3000);

    return pulsetime;
}
float getHCSR04DistanceFront(void)
{
    int pulseduration = 0;
    float calculateddistance = 0;

    /* Generate 10us pulse at P3.6 */
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN6);
    Delay(30);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN6);

    /* Wait for positive-edge (continue until echo goes high = 1) */
    while (GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN7) == 0);

    /* Start Timer - echo goes high - reset global var and count number of ticks*/
    SR04IntTimesFront = 0;
    Timer_A_clearTimer(TIMER_A0_BASE);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    /* Detects negative-edge */
    while (GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN7) == 1);

    /* Stop Timer - when echo goes low*/
    Timer_A_stopTimer(TIMER_A0_BASE);

    /* Obtain Pulse Width in microseconds */
    pulseduration = getHCSR04TimeFront();

    /* Calculating distance in cm */
    calculateddistance = (float) pulseduration / 58.0f;
    printf("Front Ultrasonic Distance: %.2fcm\n", calculateddistance);

    return calculateddistance;
}

// -----------------------------------------------------main--------------------------------------------------------------
float startUltrasonicSensor(void)
{
    Initialise_HCSR04();

    while (1)
    {
//        Delay(1000);
        getHCSR04DistanceFront();
        getHCSR04DistanceRight();
        getHCSR04DistanceLeft();

        /* Obtain distance from HCSR04 sensor and check if its less then minimum distance */

        if ((getHCSR04DistanceLeft() <= MIN_DISTANCE)){
            printf("Turn right\n");

        }
        if ((getHCSR04DistanceRight() <= MIN_DISTANCE)){
            printf("Turn left\n");

        }
        if ((getHCSR04DistanceFront() <= MIN_DISTANCE)){
            printf("Stop\n");
            printf("Reverse\n");

        }
        else{
            printf("Go straight\n");

        }

    }
}
