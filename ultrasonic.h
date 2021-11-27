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
#define MIN_DISTANCE    30.0f //30cm
#define TICKPERIOD      1000

int SR04IntTimesRight;
int SR04IntTimesLeft;
int SR04IntTimesFront;

float FrontCalculateDistance;
float RightCalculateDistance;
float LeftCalculateDistance;

// set delay eg. delay(3) 3/1000 = 0.003 seconds
static void Delay(int loop)
{
    volatile int i;
    for (i = 0; i < loop; i++);
}

// ------------------------------------------------------Configure Ultrasonice sensors -------------------------------------------------------------
void Initialise_HCSR04(void)
{
    /* Timer_A UpMode Configuration */
    // 1/1000000 x 1000 = 1ms
    const Timer_A_UpModeConfig upConfig =
    {
            TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
            TIMER_A_CLOCKSOURCE_DIVIDER_3,          // SMCLK/3 = 1MHz
            TICKPERIOD,                             // 1000 tick period
            TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
            TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,     // Enable CCR0 interrupt
            TIMER_A_DO_CLEAR                        // Clear value
    };

    // Configuring 5.2,3.6,5.0 as Output - trigger sensor
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN2);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN6);

    //prevent sensor from triggering
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN6);

    //use pull down resistor
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN7);

    // Configuring Timer_A0 for Up Mode
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);

    // Enabling interrupts and starting the timer
    Interrupt_enableInterrupt(INT_TA0_0);

    Timer_A_clearTimer(TIMER_A0_BASE);
}

// ----------------------------------------------Interrupt handler---------------------------------------------------------------------
void TA0_0_IRQHandler(void)
{
    //Every 1000 ticks = 1 interrupt
    //Increment global variable when there is 1 interrupt
    SR04IntTimesRight++;
    SR04IntTimesLeft++;
    SR04IntTimesFront++;

    /* Clear interrupt flag */
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,
    TIMER_A_CAPTURECOMPARE_REGISTER_0);
}

// ---------------------------------------------------Right Sensor----------------------------------------------------------------
static int getHCSR04TimeRight(void)
{
    int pulsetime = 0;

    //1 tick = 1 microsecond, 1000 tick = 1 millisecond
    //count total number of ticks
    pulsetime = SR04IntTimesRight * TICKPERIOD;

    //if interrupt stop halfway, Timer_A_getCounterValue will get remaining ticks and add into pulse time
    pulsetime += Timer_A_getCounterValue(TIMER_A0_BASE);

    /* Clear Timer */
    Timer_A_clearTimer(TIMER_A0_BASE);

    return pulsetime;
}

float getHCSR04DistanceRight(void)
{
    int pulseduration = 0;
    float RightCalculateDistance = 0;

    /* Generate 10us pulse at P5.0 */
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN0);
    Delay(3);
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
    RightCalculateDistance = (float) pulseduration / 58.0f;
//    printf("Right Ultrasonic Distance: %.2fcm\n", calculateddistance);

    return RightCalculateDistance;
}

// --------------------------------------------------Left Sensor-----------------------------------------------------------------
static int getHCSR04TimeLeft(void)
{
    int pulsetime = 0;

    pulsetime = SR04IntTimesLeft * TICKPERIOD;
    pulsetime += Timer_A_getCounterValue(TIMER_A0_BASE);

    Timer_A_clearTimer(TIMER_A0_BASE);

    return pulsetime;
}

float getHCSR04DistanceLeft(void)
{
    int pulseduration = 0;

    /* Generate 10us pulse at P5.2*/
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN2);
    Delay(3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);

    /* Wait for positive-edge (continue until echo goes high = 1) */
    while (GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN5) == 0)
        ;

    /* Start Timer - echo goes high - reset global var and count number of ticks*/
    SR04IntTimesLeft = 0;
    Timer_A_clearTimer(TIMER_A0_BASE);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    /* Detects negative-edge */
    while (GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN5) == 1)
        ;

    /* Stop Timer - when echo goes low*/
    Timer_A_stopTimer(TIMER_A0_BASE);

    /* Obtain Pulse Width in microseconds */
    pulseduration = getHCSR04TimeLeft();

    /* Calculating distance in cm */
    LeftCalculateDistance = (float) pulseduration / 58.0f;
//    printf("Left Ultrasonic Distance: %.2fcm\n", calculateddistance);

    return LeftCalculateDistance;
}

// --------------------------------------------------Front Sensor-----------------------------------------------------------------
static int getHCSR04TimeFront(void)
{
    int pulsetime = 0;

    pulsetime = SR04IntTimesFront * TICKPERIOD;
    pulsetime += Timer_A_getCounterValue(TIMER_A0_BASE);

    Timer_A_clearTimer(TIMER_A0_BASE);

    return pulsetime;
}

float getHCSR04DistanceFront(void)
{
    int pulseduration = 0;

    /* Generate 10us pulse at P3.6 */
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN6);
    Delay(3);
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
    FrontCalculateDistance = (float) pulseduration / 58.0f;

//    printf("Front Ultrasonic Distance: %.2fcm\n", calculateddistance);

    return FrontCalculateDistance;
}

// -----------------------------------------------------main--------------------------------------------------------------
float startUltrasonicSensor(void)
{
    Initialise_HCSR04();
    setMotorPorts();

    while (1)
    {
        //check if there is an obstacle on the left
        if (getHCSR04DistanceFront() <= MIN_DISTANCE)
        {
            //go straight
            startMoving();
            printf("Stop\n");
        };

        //check if there is an obstacle on the left
        if (getHCSR04DistanceLeft() <= MIN_DISTANCE)
        {
            //turn right
            rotateCarRight();
            printf("Turn right now\n");
        };

        //check if there is an obstacle on the right
        if (getHCSR04DistanceRight() <= MIN_DISTANCE)
        {
            //turn left
            printf("Turn left now\n");
        };

        // checks if front, left, right encounter any obstacle
        if ((getHCSR04DistanceFront() && getHCSR04DistanceLeft() && getHCSR04DistanceRight() <= MIN_DISTANCE))
        {
            //car stop
            printf("Stop now\n");
            //car reverse
            printf("Reverse now\n");
        }

        else
        {
            //move straight
            printf("Going straight now\n");

        }

    }
}
