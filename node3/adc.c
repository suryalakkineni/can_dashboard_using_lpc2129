/******************************************************************************
 * File        : adc.c
 * Controller  : LPC2129
 * Description : ADC Driver
 ******************************************************************************/

#include <LPC21xx.h>
#include "types.h"
#include "delay.h"

/******************************************************************************
 * CLOCK CONFIGURATION
 ******************************************************************************/

#define FOSC            12000000
#define CCLK            (FOSC * 5)
#define PCLK            (CCLK / 4)

#define ADCCLK          3000000
#define CLKDIV          ((PCLK / ADCCLK) - 1)

/******************************************************************************
 * ADCR REGISTER BIT DEFINITIONS
 ******************************************************************************/

#define CLKDIV_BITS         8
#define PDN_BIT             21
#define ADC_CONV_START_BIT  24

/******************************************************************************
 * ADDR REGISTER BIT DEFINITIONS
 ******************************************************************************/

#define DIGITAL_DATA_BITS   6
#define DONE_BIT            31

/******************************************************************************
 * ADC CHANNEL PIN DEFINITIONS
 ******************************************************************************/

#define AIN0_PIN_0_27   0x00400000
#define AIN1_PIN_0_28   0x01000000
#define AIN2_PIN_0_29   0x04000000
#define AIN3_PIN_0_30   0x10000000

/******************************************************************************
 * ADC CHANNEL NUMBERS
 ******************************************************************************/

#define CH0     0
#define CH1     1
#define CH2     2
#define CH3     3

/******************************************************************************
 * ADC Channel Lookup Table
 ******************************************************************************/

u32 adcChSel[4] =
{
    AIN0_PIN_0_27,
    AIN1_PIN_0_28,
    AIN2_PIN_0_29,
    AIN3_PIN_0_30
};

/******************************************************************************
 * Function : initadc()
 *
 * Purpose:
 * Initialize selected ADC channel.
 *
 * Example:
 * initadc(CH0);
 ******************************************************************************/

void initadc(u32 chNo)
{
    /* Configure selected pin as ADC input */
    PINSEL1 &= ~(adcChSel[chNo]);
    PINSEL1 |= adcChSel[chNo];

    /* Power ON ADC and set ADC clock */
    ADCR |= (1 << PDN_BIT) |
            (CLKDIV << CLKDIV_BITS);
}

/******************************************************************************
 * Function : Read_ADC()
 *
 * Purpose:
 * Read ADC value and corresponding voltage.
 *
 * Parameters:
 * chNo     -> ADC channel number
 * eAR      -> Stores voltage value
 * adcDVal  -> Stores raw ADC value (0-1023)
 ******************************************************************************/

void Read_ADC(u32 chNo, f32 *eAR, u32 *adcDVal)
{
    /* Clear previous channel selection */
    ADCR &= 0xFFFFFF00;

    /* Select channel and start conversion */
    ADCR |= (1 << ADC_CONV_START_BIT) |
            (1 << chNo);

    /* ADC conversion delay */
    delay_us(3);

    /* Wait until conversion completes */
    while(((ADDR >> DONE_BIT) & 1) == 0);

    /* Stop conversion */
    ADCR &= ~(1 << ADC_CONV_START_BIT);

    /* Read 10-bit ADC result */
    *adcDVal = (ADDR >> DIGITAL_DATA_BITS) & 0x3FF;

    /* Convert ADC value into voltage */
    *eAR = (*adcDVal) * (3.3 / 1023.0);
}
