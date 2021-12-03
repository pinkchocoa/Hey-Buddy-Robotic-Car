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
#pragma once
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>

#define MIN_DISTANCE    15.0f // 5 cm
#define LR_MIN_DISTANCE 10.0f // 8 cm
#define TICKPERIOD      1000  // to get 1ms interrupt

int SR04IntTimesRight;
int SR04IntTimesLeft;
int SR04IntTimesFront;

bool check;

// set delay in milliseconds
static void Delay(int loop)
{
    volatile int i;
    for (i = 0; i < loop; i++);
}

//Initialise Ultrasonic Sensors
void initUltraSensors(void)
{
    /* Timer_A UpMode Configuration Parameter */
    const Timer_A_UpModeConfig upConfig = {
                 TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
                 TIMER_A_CLOCKSOURCE_DIVIDER_3,          // SMCLK/3 = 1MHz
                 TICKPERIOD,                             // 1000 tick period
                 TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
                 TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,     // Enable CCR0 interrupt
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
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);

    /* Enabling interrupts and starting the timer */
    Interrupt_enableInterrupt(INT_TA1_0);

    //Timer_A_stopTimer(TIMER_A0_BASE);
    Timer_A_clearTimer(TIMER_A0_BASE);
}
//Interrupt Handler - every 1000 ticks, call ISR
void TA1_0_IRQHandler(void)
{
    //count number of interrupt
    SR04IntTimesRight++;
    SR04IntTimesLeft++;
    SR04IntTimesFront++;

    //clear interrupt flag
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,
                                         TIMER_A_CAPTURECOMPARE_REGISTER_0);
}

//Right sensor
static int getHCSR04TimeRight(void)
{
    int pulsetime = 0;

    /* Calculate total number of ticks (1 interrupt = 1000 ticks)    */
    pulsetime = SR04IntTimesRight * TICKPERIOD;

    /* If interrupt stopped halfway, add remaining pulse into with initial    */
    pulsetime += Timer_A_getCounterValue(TIMER_A0_BASE);

    /* Clear Timer */
    Timer_A_clearTimer(TIMER_A0_BASE);

    return pulsetime;
}

float getHCSR04DistanceRight(void)
{
    int pulseduration = 0;
    float RightCalculatedDistance = 0;

    /* Generate at least 10us pulse at P5.0 */
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN0);
    //3ms/1000 = 0.03sec
    Delay(3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0);

    /* Detects positive-edge - when echo received*/
    while (GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN1) == 0);

    /* Start Timer - clear global var, clear timer, start counter */
    SR04IntTimesRight = 0;
    Timer_A_clearTimer(TIMER_A0_BASE);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    /* Detects negative-edge - when echo finished*/
    while (GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN1) == 1);

    /* Stop Timer */
    Timer_A_stopTimer(TIMER_A0_BASE);

    /* Obtain Pulse Width in microseconds */
    pulseduration = getHCSR04TimeRight();

    /* Calculating distance in cm */
    RightCalculatedDistance = (float) pulseduration / 58.0f;
//    printf("Right Ultrasonic Distance: %.2fcm\n", RightCalculatedDistance);

    return RightCalculatedDistance;
}

// Left sensor
static int getHCSR04TimeLeft(void)
{
    int pulsetime = 0;

    /* Number of times the interrupt occurred (1 interrupt = 1000 ticks)    */
    pulsetime = SR04IntTimesLeft * TICKPERIOD;

    /* If interrupt stopped halfway, add remaining pulse into with initial    */
    pulsetime += Timer_A_getCounterValue(TIMER_A0_BASE);

    /* Clear Timer */
    Timer_A_clearTimer(TIMER_A0_BASE);

    return pulsetime;
}
float getHCSR04DistanceLeft(void)
{
    int pulseduration = 0;
    float LeftCalculatedDistance = 0;

    /* Generate 10us pulse at P5.2 */
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN2);
    Delay(3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);

    /* Detects positive-edge - when echo received*/
    while (GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN5) == 0);

    /* Start Timer - clear global var, clear timer, start counter */
    SR04IntTimesLeft = 0;
    Timer_A_clearTimer(TIMER_A0_BASE);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    /* Detects negative-edge - when echo finished*/
    while (GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN5) == 1);

    /* Stop Timer */
    Timer_A_stopTimer(TIMER_A0_BASE);

    /* Obtain Pulse Width in microseconds */
    pulseduration = getHCSR04TimeLeft();

    /* Calculating distance in cm */
    LeftCalculatedDistance = (float) pulseduration / 58.0f;
//    printf("Left Ultrasonic Distance: %.2fcm\n", LeftCalculatedDistance);

    return LeftCalculatedDistance;
}

// Front sensor
static int getHCSR04TimeFront(void)
{
    int pulsetime = 0;

    /* Number of times the interrupt occurred (1 interrupt = 1000 ticks)    */
    pulsetime = SR04IntTimesFront * TICKPERIOD;

    /* If interrupt stopped halfway, add remaining pulse into with initial    */
    pulsetime += Timer_A_getCounterValue(TIMER_A0_BASE);

    /* Clear Timer */
    Timer_A_clearTimer(TIMER_A0_BASE);

    return pulsetime;
}

float getHCSR04DistanceFront(void)
{
    int pulseduration = 0;
    float FrontCalculatedDistance = 0;

    /* Generate 10us pulse at P3.6 */
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN6);
    Delay(3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN6);

    /* Detects positive-edge - when echo received*/
    while (GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN7) == 0);

    /* Start Timer - clear global var, clear timer, start counter */
    SR04IntTimesFront = 0;
    Timer_A_clearTimer(TIMER_A0_BASE);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    /* Detects negative-edge - when echo finished*/
    while (GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN7) == 1);

    /* Stop Timer */
    Timer_A_stopTimer(TIMER_A0_BASE);

    /* Obtain Pulse Width in microseconds */
    pulseduration = getHCSR04TimeFront();

    /* Calculating distance in cm */
    FrontCalculatedDistance = (float) pulseduration / 58.0f;
//    printf("Front Ultrasonic Distance: %.2fcm\n", FrontCalculatedDistance);

    return FrontCalculatedDistance;
}


