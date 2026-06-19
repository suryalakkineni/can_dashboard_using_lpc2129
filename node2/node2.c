/******************************************************************************
 * Project    : CAN Dashboard System
 * File       : node2.c
 * Controller : LPC2129
 *
 * DESCRIPTION:
 * Indicator Node
 *
 * Receives indicator commands from Main Node through CAN bus
 * and controls LEDs connected to P0.0-P0.7.
 *
 * CAN DATA:
 * ---------
 * 'L' -> LEFT Indicator
 * 'R' -> RIGHT Indicator
 * 'O' -> Indicator OFF
 *
 ******************************************************************************/

#include <LPC21xx.h>

#include "types.h"
#include "delay.h"
#include "can.h"

/* Indicator States */
#define OFF     0
#define LEFT    1
#define RIGHT   2

/* LEDs Connected to P0.0 - P0.7 */
#define LED_MASK    0xFF

/* Global Variables */
CANF indicatorRxFrame;

volatile u8 indicatorMode = OFF;
volatile u8 ledPosition = 0;

/******************************************************************************
 * FUNCTION:
 * LeftIndicatorPattern()
 *
 * LED Movement:
 * LED7 -> LED6 -> LED5 -> LED4 -> LED3 -> LED2 -> LED1 -> LED0
 ******************************************************************************/
void LeftIndicatorPattern(void)
{
    /* Turn OFF All LEDs */
    IOSET0 = LED_MASK;

    if(ledPosition == 0)
        ledPosition = 7;
    else
        ledPosition--;

    /* Turn ON Current LED */
    IOCLR0 = (1 << ledPosition);
}

/******************************************************************************
 * FUNCTION:
 * RightIndicatorPattern()
 *
 * LED Movement:
 * LED0 -> LED1 -> LED2 -> LED3 -> LED4 -> LED5 -> LED6 -> LED7
 ******************************************************************************/
void RightIndicatorPattern(void)
{
    /* Turn OFF All LEDs */
    IOSET0 = LED_MASK;

    /* Turn ON Current LED */
    IOCLR0 = (1 << ledPosition);

    ledPosition++;

    if(ledPosition >= 8)
        ledPosition = 0;
}

/******************************************************************************
 * MAIN FUNCTION
 ******************************************************************************/
int main(void)
{
    /* Initialize CAN Controller */
    Init_CAN1();

    /* Configure P0.0-P0.7 as Output */
    IODIR0 |= LED_MASK;

    /* Turn OFF All LEDs Initially */
    IOSET0 = LED_MASK;

    while(1)
    {
        /******************************************************************
         * Receive CAN Message
         ******************************************************************/
        if(C1GSR & (1 << 0))
        {
            CAN1_Rx(&indicatorRxFrame);

            /* Release CAN Receive Buffer */
            C1CMR = (1 << 2);

            if(indicatorRxFrame.ID == 0x11)
            {
                if(indicatorRxFrame.Data1 == 'L')
                {
                    indicatorMode = LEFT;
                    ledPosition = 7;
                }
                else if(indicatorRxFrame.Data1 == 'R')
                {
                    indicatorMode = RIGHT;
                    ledPosition = 0;
                }
                else if(indicatorRxFrame.Data1 == 'O')
                {
                    indicatorMode = OFF;
                    ledPosition = 0;

                    /* Turn OFF All LEDs */
                    IOSET0 = LED_MASK;
                }
            }
        }

        /******************************************************************
         * Execute Indicator Pattern
         ******************************************************************/
        if(indicatorMode == LEFT)
        {
            LeftIndicatorPattern();
        }
        else if(indicatorMode == RIGHT)
        {
            RightIndicatorPattern();
        }
        else
        {
            IOSET0 = LED_MASK;
        }

        delay_ms(100);
    }
}

