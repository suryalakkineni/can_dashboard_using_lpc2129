#include <LPC21xx.h>

#include "types.h"
#include "lcd.h"
#include "adc.h"
#include "delay.h"

int main()
{
    u16 adcval;
    u16 fuel;

    float eAR;

    /* Initialize LCD */

    LCD_Init();

    /* Initialize ADC */

    Init_ADC();

    /* Welcome Message */

    Write_CMD_LCD(0x01);

    Write_str_LCD("Fuel Monitor");

    delay_ms(1000);

    Write_CMD_LCD(0x01);

    while(1)
    {
        /* Read ADC from AD0.0 */

        Read_ADC(0,
                 &adcval,
                 &eAR);

        /* Convert ADC to Fuel Percentage */

        fuel =
        (adcval * 100) / 1023;

        if(fuel > 100)
        {
            fuel = 100;
        }

        /* First Line */

        Write_CMD_LCD(0x80);

        Write_str_LCD("Fuel:");

        Write_int_LCD(fuel);

        Write_DAT_LCD('%');

        Write_str_LCD("   ");

        /* Second Line */

        Write_CMD_LCD(0xC0);

        Write_str_LCD("ADC:");

        Write_int_LCD(adcval);

        Write_str_LCD("   ");

        delay_ms(200);
    }
}