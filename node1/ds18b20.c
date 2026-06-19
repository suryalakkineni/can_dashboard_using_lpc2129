/******************************************************************************
 * File        : ds18b20.c
 * Controller  : LPC2129
 * Sensor      : DS18B20 Digital Temperature Sensor
 * Interface   : 1-Wire Protocol
 *
 * Description:
 * ------------
 * This file contains low-level driver functions for interfacing the
 * DS18B20 temperature sensor with LPC2129 using the 1-Wire protocol.
 *
 * Data Pin Connection:
 * --------------------
 * DS18B20 DQ  ---> P0.15
 * DS18B20 VDD ---> +5V / +3.3V
 * DS18B20 GND ---> GND
 *
 ******************************************************************************/

#include <LPC21xx.h>
#include "delay.h"

/*------------------------------------------------------------
  DS18B20 Data Line Connected to P0.15
------------------------------------------------------------*/
#define D   (1 << 15)

/* Read Current Status of Data Line */
#define R   (IOPIN0 & (1 << 15))

/******************************************************************************
 * Function : ResetDS18b20()
 *
 * Purpose:
 * --------
 * Sends Reset Pulse to DS18B20 and checks for Presence Pulse.
 *
 * Return:
 * --------
 * 0 -> Sensor Present
 * 1 -> Sensor Not Present
 *
 ******************************************************************************/
unsigned char ResetDS18b20(void)
{
    unsigned int presence;

    /* Configure P0.15 as Output */
    IODIR0 |= D;

    /* Drive Bus High */
    IOPIN0 |= D;

    delay_us(1);

    /* Generate Reset Pulse */
    IOPIN0 &= ~D;

    /* Hold Low for 480us */
    delay_us(478);

    /* Release Bus */
    IOPIN0 |= D;

    /* Wait for Presence Pulse */
    delay_us(54);

    /* Read Presence Status */
    presence = IOPIN0;

    /* Wait for Completion */
    delay_us(423);

    /* Check Presence Pulse */
    if(presence & R)
        return 1;       /* No Sensor Found */
    else
        return 0;       /* Sensor Present */
}

/******************************************************************************
 * Function : ReadBit()
 *
 * Purpose:
 * --------
 * Reads a single bit from DS18B20.
 *
 * Return:
 * --------
 * 0 or 1
 *
 ******************************************************************************/
unsigned char ReadBit(void)
{
    unsigned int bitValue;

    /* Pull Bus Low */
    IOPIN0 &= ~D;

    delay_us(1);

    /* Release Bus */
    IOPIN0 |= D;

    /* Configure as Input */
    IODIR0 &= ~D;

    /* Wait before Sampling */
    delay_us(10);

    /* Read Data Line */
    bitValue = IOPIN0;

    /* Restore Output Mode */
    IODIR0 |= D;

    if(bitValue & R)
        return 1;
    else
        return 0;
}

/******************************************************************************
 * Function : WriteBit()
 *
 * Purpose:
 * --------
 * Writes a single bit to DS18B20.
 *
 * Parameters:
 * -----------
 * Dbit = 0 or 1
 *
 ******************************************************************************/
void WriteBit(unsigned char Dbit)
{
    /* Start Time Slot */
    IOPIN0 &= ~D;

    delay_us(1);

    /* Write Logic 1 */
    if(Dbit)
        IOPIN0 |= D;

    /* Complete Time Slot */
    delay_us(58);

    /* Release Bus */
    IOPIN0 |= D;

    delay_us(1);
}

/******************************************************************************
 * Function : ReadByte()
 *
 * Purpose:
 * --------
 * Reads one complete byte from DS18B20.
 *
 * Return:
 * --------
 * Received byte
 *
 ******************************************************************************/
unsigned char ReadByte(void)
{
    unsigned char i;
    unsigned char data = 0;

    for(i = 0; i < 8; i++)
    {
        if(ReadBit())
        {
            data |= (1 << i);
        }

        delay_us(45);
    }

    return data;
}

/******************************************************************************
 * Function : WriteByte()
 *
 * Purpose:
 * --------
 * Writes one complete byte to DS18B20.
 *
 * Parameters:
 * -----------
 * Dout -> Data byte to send
 *
 ******************************************************************************/
void WriteByte(unsigned char Dout)
{
    unsigned char i;

    for(i = 0; i < 8; i++)
    {
        /* Send LSB First */
        WriteBit(Dout & 0x01);

        Dout >>= 1;

        delay_us(1);
    }

    delay_us(98);
}

/******************************************************************************
 * Function : ReadTemp()
 *
 * Purpose:
 * --------
 * Reads temperature value from DS18B20.
 *
 * DS18B20 Commands:
 * -----------------
 * 0xCC -> Skip ROM
 * 0x44 -> Start Temperature Conversion
 * 0xBE -> Read Scratchpad
 *
 * Return:
 * --------
 * Raw 16-bit Temperature Value
 *
 * Temperature Conversion:
 * -----------------------
 * float temperature;
 *
 * temperature = ReadTemp() / 16.0;
 *
 ******************************************************************************/
int ReadTemp(void)
{
    unsigned char n;
    unsigned char buff[2];

    int temp;

    /*-------------------------------
      Start Temperature Conversion
    -------------------------------*/
    ResetDS18b20();

    WriteByte(0xCC);     /* Skip ROM */

    WriteByte(0x44);     /* Convert Temperature */

    /* Wait Until Conversion Complete */
    while(ReadByte() == 0xFF);

    /*-------------------------------
      Read Scratchpad
    -------------------------------*/
    ResetDS18b20();

    WriteByte(0xCC);     /* Skip ROM */

    WriteByte(0xBE);     /* Read Scratchpad */

    /* Read Temperature LSB and MSB */
    for(n = 0; n < 2; n++)
    {
        buff[n] = ReadByte();
    }

    /* Combine MSB and LSB */
    temp = buff[1];

    temp <<= 8;

    temp |= buff[0];

    return temp;
}

