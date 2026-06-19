/******************************************************************************
 * File        : ds18b20.h
 * Description : DS18B20 Temperature Sensor Function Prototypes
 ******************************************************************************/

#ifndef DS18B20_H
#define DS18B20_H

#include "types.h"

/* Reset DS18B20 and check presence pulse */
u8 ResetDS18b20(void);

/* Read one bit from DS18B20 */
unsigned char ReadBit(void);

/* Write one bit to DS18B20 */
void WriteBit(unsigned char DataBit);

/* Read one byte from DS18B20 */
unsigned char ReadByte(void);

/* Write one byte to DS18B20 */
void WriteByte(unsigned char DataOut);

/* Read temperature from DS18B20 */
int ReadTemp(void);

#endif /* DS18B20_H */

