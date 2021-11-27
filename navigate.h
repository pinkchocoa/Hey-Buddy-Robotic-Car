#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#include "movement.h"
#include "ultrasonic.h"

#define MIN_DISTANCE    30.0f //30 cm
// getHCSR04DistanceFront()
// getHCSR04DistanceLeft()
// getHCSR04DistanceRight()

void navigate(){
    
    // going straight first if there is an obstacle infront
    if (getHCSR04DistanceFront()  <= MIN_DISTANCE ){

        //check if there is an obstacle on the left
        if(getHCSR04DistanceLeft() <= MIN_DISTANCE ){
            //turn right
            printf("Turn right now");
        };

        //check if there is an obstacle on the right
        if(getHCSR04DistanceRight() <= MIN_DISTANCE ){
            //turn left
             printf("Turn left now");

        };
    }
    // checks if front, left, right any obstacle
    if ((getHCSR04DistanceFront() && getHCSR04DistanceLeft() && getHCSR04DistanceRight() <= MIN_DISTANCE){
        //car stop
        printf("Stop now");
        //car reverse
        printf("Reverse now");
         

    }
    else{
        //move straight
        printf("Going straight now");

    }

}