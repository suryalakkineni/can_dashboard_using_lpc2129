/******************************************************************************
 * Project    : CAN Dashboard System
 * File       : node1.c
 * Controller : LPC2129
 *
 * DESCRIPTION:
 * Main Dashboard Node
 *
 * FUNCTIONS:
 * 1. Reads engine temperature from DS18B20 sensor.
 * 2. Receives fuel percentage from Fuel Node through CAN.
 * 3. Displays Fuel, Temperature and Indicator status on LCD.
 * 4. Detects LEFT and RIGHT switch presses using interrupts.
 * 5. Sends indicator status to Indicator Node through CAN.
 *
 * HARDWARE CONNECTIONS
 * --------------------
 * LCD D0-D7  -> P0.0  to P0.7
 * LCD RW     -> P0.9
 * LCD EN     -> P0.10
 * LCD RS     -> P0.11
 *
 * LEFT SW    -> P0.14 (EINT0)
 * DS18B20    -> P0.15
 * RIGHT SW   -> P0.16 (EINT1)
 *
 * CAN TXD1   -> P0.24
 * CAN RXD1   -> P0.25
 ******************************************************************************/

#include <LPC21xx.h>

#include "types.h"
#include "lcd.h"
#include "delay.h"
#include "ds18b20.h"
#include "can.h"

/******************************************************************************
 * Indicator State Definitions
 ******************************************************************************/
#define OFF     0
#define LEFT    1
#define RIGHT   2

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/* Current Indicator State */
volatile u8 indicatorMode = OFF;

/* CAN Frames */
CANF dashboardTxFrame;
CANF dashboardRxFrame;

/* Temperature Variables */
int engineTempRaw;
u8 engineTempC;

/******************************************************************************
 * Custom LCD Arrow Symbols
 ******************************************************************************/

/* Left Arrow Character */
u8 leftIndicatorIcon[8] =
{
    0x03,
    0x07,
    0x0F,
    0x1F,
    0x0F,
    0x07,
    0x03,
    0x01
};

/* Right Arrow Character */
u8 rightIndicatorIcon[8] =
{
    0x18,
    0x1C,
    0x1E,
    0x1F,
    0x1E,
    0x1C,
    0x18,
    0x10
};

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/

void LoadIndicatorIcons(void);
void SendIndicatorCommand(void);

void LeftSwitch_ISR(void) __irq;
void RightSwitch_ISR(void) __irq;

/******************************************************************************
 * FUNCTION : LoadIndicatorIcons()
 *
 * DESCRIPTION:
 * Loads custom arrow symbols into LCD CGRAM.
 *
 * Character 5 -> Left Arrow
 * Character 6 -> Right Arrow
 ******************************************************************************/
void LoadIndicatorIcons(void)
{
    u8 index;

    /* Load Left Arrow into LCD CGRAM Location 5 */
    CmdLCD(0x68);

    for(index = 0; index < 8; index++)
    {
        CharLCD(leftIndicatorIcon[index]);
    }

    /* Load Right Arrow into LCD CGRAM Location 6 */
    CmdLCD(0x70);

    for(index = 0; index < 8; index++)
    {
        CharLCD(rightIndicatorIcon[index]);
    }
}

/******************************************************************************
 * FUNCTION : SendIndicatorCommand()
 *
 * DESCRIPTION:
 * Sends Indicator Status to Indicator Node via CAN Bus.
 *
 * Data Sent:
 * ----------
 * 'L' -> Left Indicator ON
 * 'R' -> Right Indicator ON
 * 'O' -> Indicator OFF
 ******************************************************************************/
void SendIndicatorCommand(void)
{
    /* CAN Message Identifier */
    dashboardTxFrame.ID = 0x11;

    /* Standard Data Frame */
    dashboardTxFrame.BFV.RTR = 0;

    /* One Byte Data Length */
    dashboardTxFrame.BFV.DLC = 1;

    if(indicatorMode == LEFT)
    {
        dashboardTxFrame.Data1 = 'L';
    }
    else if(indicatorMode == RIGHT)
    {
        dashboardTxFrame.Data1 = 'R';
    }
    else
    {
        dashboardTxFrame.Data1 = 'O';
    }

    /* Transmit CAN Frame */
    CAN1_Tx(dashboardTxFrame);
}

/******************************************************************************
 * INTERRUPT SERVICE ROUTINE
 *
 * FUNCTION:
 * LeftSwitch_ISR()
 *
 * INTERRUPT:
 * EINT0 (P0.14)
 *
 * DESCRIPTION:
 * Executes when LEFT switch is pressed.
 * Toggles LEFT indicator state and
 * sends updated status through CAN.
 ******************************************************************************/
void LeftSwitch_ISR(void) __irq
{
    if(indicatorMode == LEFT)
    {
        indicatorMode = OFF;
    }
    else
    {
        indicatorMode = LEFT;
    }

    /* Send Updated Indicator Status */
    SendIndicatorCommand();

    /* Clear EINT0 Interrupt Flag */
    EXTINT = (1 << 0);

    /* End Interrupt */
    VICVectAddr = 0;
}

/******************************************************************************
 * INTERRUPT SERVICE ROUTINE
 *
 * FUNCTION:
 * RightSwitch_ISR()
 *
 * INTERRUPT:
 * EINT1 (P0.16)
 *
 * DESCRIPTION:
 * Executes when RIGHT switch is pressed.
 * Toggles RIGHT indicator state and
 * sends updated status through CAN.
 ******************************************************************************/
void RightSwitch_ISR(void) __irq
{
    if(indicatorMode == RIGHT)
    {
        indicatorMode = OFF;
    }
    else
    {
        indicatorMode = RIGHT;
    }

    /* Send Updated Indicator Status */
    SendIndicatorCommand();

    /* Clear EINT1 Interrupt Flag */
    EXTINT = (1 << 1);

    /* End Interrupt */
    VICVectAddr = 0;
}

/******************************************************************************
 * FUNCTION : main()
 *
 * DESCRIPTION:
 * Main Control Function of Dashboard Node.
 *
 * INITIALIZATION:
 * ---------------
 * 1. LCD Initialization
 * 2. CAN Initialization
 * 3. Load Custom Arrow Symbols
 * 4. Configure External Interrupts
 *
 * MAIN LOOP:
 * ----------
 * 1. Animate Indicator Arrows
 * 2. Read Temperature from DS18B20
 * 3. Receive Fuel Percentage via CAN
 * 4. Update LCD Display
 ******************************************************************************/
int main(void)
{
    /**********************************************************************
     * Initialize LCD Module
     **********************************************************************/
    InitLCD();

    /**********************************************************************
     * Initialize CAN Controller
     **********************************************************************/
    Init_CAN1();

    /**********************************************************************
     * Display Dashboard Title
     **********************************************************************/
    CmdLCD(0x80);
    StrLCD("<<< DASH BOARD >>>");

    /**********************************************************************
     * Display Indicator Label
     **********************************************************************/
    CmdLCD(0xC0);
    StrLCD("Indicator");

    /**********************************************************************
     * Load Arrow Symbols Into LCD CGRAM
     **********************************************************************/
    LoadIndicatorIcons();

    /**********************************************************************
     * Display Initial Arrow Symbols
     **********************************************************************/
    CmdLCD(0xC0 + 13);
    CharLCD(5);

    CmdLCD(0xC0 + 15);
    CharLCD(6);

    /**********************************************************************
     * Configure External Interrupt Pins
     *
     * P0.14 -> EINT0 (LEFT SWITCH)
     * P0.16 -> EINT1 (RIGHT SWITCH)
     **********************************************************************/

    /* Configure P0.14 as EINT0 */
    PINSEL0 &= ~(3 << 28);
    PINSEL0 |=  (2 << 28);

    /* Configure P0.16 as EINT1 */
    PINSEL1 &= ~(3 << 0);
    PINSEL1 |=  (1 << 0);

    /**********************************************************************
     * Configure Edge Triggered Interrupt Mode
     **********************************************************************/
    EXTMODE |= (1 << 0) | (1 << 1);

    /**********************************************************************
     * Configure Falling Edge Trigger
     *
     * Switch Press:
     * HIGH -> LOW
     **********************************************************************/
    EXTPOLAR &= ~((1 << 0) | (1 << 1));

    /**********************************************************************
     * Clear Pending Interrupt Flags
     **********************************************************************/
    EXTINT = (1 << 0) | (1 << 1);

    /**********************************************************************
     * Configure VIC For EINT0
     **********************************************************************/
    VICVectCntl0 = (1 << 5) | 14;
    VICVectAddr0 = (u32)LeftSwitch_ISR;

    /**********************************************************************
     * Configure VIC For EINT1
     **********************************************************************/
    VICVectCntl1 = (1 << 5) | 15;
    VICVectAddr1 = (u32)RightSwitch_ISR;

    /**********************************************************************
     * Enable EINT0 And EINT1 Interrupts
     **********************************************************************/
    VICIntEnable = (1 << 14) | (1 << 15);

    /**********************************************************************
     * Infinite Loop
     **********************************************************************/
    while(1)
    {
        /******************************************************************
         * LEFT Indicator Animation
         ******************************************************************/
        if(indicatorMode == LEFT)
        {
            CmdLCD(0xC0 + 13);
            CharLCD(5);

            delay_ms(300);

            CmdLCD(0xC0 + 13);
            CharLCD(' ');

            CmdLCD(0xC0 + 15);
            CharLCD(6);
        }

        /******************************************************************
         * RIGHT Indicator Animation
         ******************************************************************/
        else if(indicatorMode == RIGHT)
        {
            CmdLCD(0xC0 + 13);
            CharLCD(5);

            CmdLCD(0xC0 + 15);
            CharLCD(6);

            delay_ms(300);

            CmdLCD(0xC0 + 15);
            CharLCD(' ');
        }

        /******************************************************************
         * Indicator OFF State
         ******************************************************************/
        else
        {
            CmdLCD(0xC0 + 13);
            CharLCD(5);

            CmdLCD(0xC0 + 15);
            CharLCD(6);
        }

        /******************************************************************
         * Read Engine Temperature
         *
         * DS18B20 Returns:
         * Temperature × 16
         *
         * Example:
         * 25°C -> 25 << 4
         ******************************************************************/
        engineTempRaw = ReadTemp();

        /* Convert To Celsius */
        engineTempC = engineTempRaw >> 4;

        /******************************************************************
         * Display Temperature
         ******************************************************************/
        CmdLCD(0xD4);
        StrLCD("Temp : ");
        IntLCD(engineTempC);
        StrLCD(" C   ");

        /******************************************************************
         * Receive Fuel Percentage From Fuel Node
         ******************************************************************/
        CAN1_Rx(&dashboardRxFrame);

        /******************************************************************
         * Display Fuel Percentage
         ******************************************************************/
        CmdLCD(0x94);
				
        StrLCD("Fuel : ");
			

        IntLCD(dashboardRxFrame.Data1);

        CharLCD('%');

        StrLCD("   ");

        /******************************************************************
         * Small Delay For Stable LCD Update
         ******************************************************************/
        delay_ms(100);
    }
}

