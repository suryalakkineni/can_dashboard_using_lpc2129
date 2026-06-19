/******************************************************************************
 * File        : lcd.c
 * Controller  : LPC2129 / LPC2148
 * Description : 8-bit LCD Driver (16x2 / 20x4 Compatible)
 *
 * LCD Connections:
 * ----------------
 * LCD D0-D7  -> P0.0 - P0.7
 * LCD RW     -> P0.9
 * LCD EN     -> P0.10
 * LCD RS     -> P0.11
 *
 ******************************************************************************/

#include <LPC21xx.h>
#include "delay.h"
#include "LCD.h"
#include "types.h"
#include "defines.h"

/* LCD Data Bus Start Position */
#define LCD_DAT    0      /* P0.0 - P0.7 */

/* LCD Control Pins */
#define RW         9      /* P0.9 */
#define EN         10     /* P0.10 */
#define RS         11     /* P0.11 */


/******************************************************************************
 * Function    : InitLCD
 * Description : Initialize LCD in 8-bit mode
 ******************************************************************************/
void InitLCD(void)
{
    /* Configure RS, RW and EN pins as Output */
    IODIR0 |= (1<<RS) | (1<<RW) | (1<<EN);

    /* Configure P0.0-P0.7 as LCD Data Bus */
    IODIR0 |= (0xFF << LCD_DAT);

    /* LCD Power ON Delay */
    delay_ms(20);

    /* LCD Initialization Sequence */
    CmdLCD(0x30);
    delay_ms(10);

    CmdLCD(0x30);
    delay_ms(1);

    CmdLCD(0x30);
    delay_ms(1);

    /* Function Set:
       8-bit Interface
       2 Line Display
       5x7 Font */
    CmdLCD(0x38);

    /* Display OFF */
    CmdLCD(0x08);

    /* Clear LCD */
    CmdLCD(0x01);

    /* Entry Mode:
       Cursor Increment */
    CmdLCD(0x06);

    /* Display ON
       Cursor OFF */
    CmdLCD(0x0C);
}


/******************************************************************************
 * Function    : CmdLCD
 * Description : Send Command to LCD
 ******************************************************************************/
void CmdLCD(u8 cmd)
{
    /* RS = 0 -> Command Register */
    IOCLR0 = (1<<RS);

    /* Send Command */
    DispLCD(cmd);
}


/******************************************************************************
 * Function    : CharLCD
 * Description : Display One Character on LCD
 ******************************************************************************/
void CharLCD(u8 dat)
{
    /* RS = 1 -> Data Register */
    IOSET0 = (1<<RS);

    /* Send Data */
    DispLCD(dat);
}


/******************************************************************************
 * Function    : DispLCD
 * Description : Send 8-bit Data/Command to LCD
 ******************************************************************************/
void DispLCD(u8 val)
{
    /* RW = 0 -> Write Operation */
    IOCLR0 = (1<<RW);

    /* Clear Previous Data */
    IOCLR0 = (0xFF << LCD_DAT);

    /* Put New Data on LCD Bus */
    IOSET0 = ((u32)val << LCD_DAT);

    /* Generate Enable Pulse */
    IOSET0 = (1<<EN);
    delay_ms(2);

    IOCLR0 = (1<<EN);
    delay_ms(2);
}


/******************************************************************************
 * Function    : StrLCD
 * Description : Display String on LCD
 ******************************************************************************/
void StrLCD(unsigned char *ptr)
{
    while(*ptr)
    {
        CharLCD(*ptr++);
    }
}


/******************************************************************************
 * Function    : IntLCD
 * Description : Display Signed Integer on LCD
 ******************************************************************************/
void IntLCD(s32 num)
{
    u8 a[10];
    s8 i = 0;

    /* Special Case for Zero */
    if(num == 0)
    {
        CharLCD('0');
        return;
    }

    /* Handle Negative Number */
    if(num < 0)
    {
        CharLCD('-');
        num = -num;
    }

    /* Convert Number to ASCII */
    while(num)
    {
        a[i++] = (num % 10) + '0';
        num /= 10;
    }

    /* Display Digits in Correct Order */
    while(i)
    {
        CharLCD(a[--i]);
    }
}


/******************************************************************************
 * Function    : FltLCD
 * Description : Display Floating Point Number
 * Example     : 23.45
 ******************************************************************************/
void FltLCD(f32 val)
{
    int integer;
    int decimal;

    integer = (int)val;

    decimal = (int)((val - integer) * 100);

    IntLCD(integer);

    CharLCD('.');

    if(decimal < 10)
    {
        CharLCD('0');
    }

    IntLCD(decimal);
}


/******************************************************************************
 * Function    : CgramLCD
 * Description : Clear LCD
 ******************************************************************************/
void CgramLCD(void)
{
    CmdLCD(0x01);
}


/******************************************************************************
 * Function    : GoToLCD
 * Description : Move Cursor to Desired Position
 *
 * row = 0 -> First Line
 * row = 1 -> Second Line
 * row = 2 -> Third Line
 * row = 3 -> Fourth Line
 ******************************************************************************/
void GoToLCD(u8 row, u8 col)
{
    switch(row)
    {
        case 0:
            CmdLCD(0x80 + col);
            break;

        case 1:
            CmdLCD(0xC0 + col);
            break;

        case 2:
            CmdLCD(0x94 + col);
            break;

        case 3:
            CmdLCD(0xD4 + col);
            break;

        default:
            break;
    }
}


/******************************************************************************
 * Function    : StoreCustCharFont
 * Description : Store Fuel Level Symbols in CGRAM
 *
 * Character Codes:
 * ----------------
 * 0 -> Empty Fuel Tank
 * 1 -> 25% Fuel
 * 2 -> 50% Fuel
 * 3 -> 75% Fuel
 * 4 -> Full Fuel Tank
 ******************************************************************************/


/******************************************************************************
 * Function    : DisplayFuel
 * Description : Display Fuel Icon and Percentage
 ******************************************************************************/
void DisplayFuel(u8 fuel_percent)
{
    GoToLCD(1,0);

    StrLCD("Fuel:");

    if(fuel_percent < 20)
        CharLCD(0);      /* Empty */

    else if(fuel_percent < 40)
        CharLCD(1);      /* 25% */

    else if(fuel_percent < 60)
        CharLCD(2);      /* 50% */

    else if(fuel_percent < 80)
        CharLCD(3);      /* 75% */

    else
        CharLCD(4);      /* Full */

    CharLCD(' ');

    IntLCD(fuel_percent);

    CharLCD('%');
}

/******************************************************************************
 * Example:
 *
 * InitLCD();
 * StoreCustCharFont();
 *
 * DisplayFuel(72);
 *
 * LCD Output:
 *
 * Fuel: [75% Icon] 72%
 *
 ******************************************************************************/
