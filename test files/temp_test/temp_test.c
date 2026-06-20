#include <LPC21xx.h>

/*==================== DELAY FUNCTIONS ====================*/
void DelayUs(unsigned int us)
{
    unsigned int i;
    while(us--)
        for(i=0;i<15;i++);
}

void delay_ms(unsigned int ms)
{
    unsigned int i;
    while(ms--)
        for(i=0;i<6000;i++);
}

/*==================== LCD DEFINITIONS ====================*/
#define LCD_DATA   0xFF        // P0.0 - P0.7
#define RS         (1<<17)
#define RW         (1<<18)
#define EN         (1<<19)

/*==================== LCD FUNCTIONS ====================*/
void LCD_Command(unsigned char cmd)
{
    IOCLR0 = LCD_DATA;
    IOSET0 = cmd;

    IOCLR0 = RS;
    IOCLR0 = RW;

    IOSET0 = EN;
    delay_ms(2);
    IOCLR0 = EN;
}

void LCD_Data(unsigned char data)
{
    IOCLR0 = LCD_DATA;
    IOSET0 = data;

    IOSET0 = RS;
    IOCLR0 = RW;

    IOSET0 = EN;
    delay_ms(2);
    IOCLR0 = EN;
}

void LCD_String(char *str)
{
    while(*str)
        LCD_Data(*str++);
}

void LCD_Init(void)
{
    IODIR0 |= LCD_DATA | RS | RW | EN;

    delay_ms(20);

    LCD_Command(0x38);
    LCD_Command(0x0C);
    LCD_Command(0x01);
    LCD_Command(0x06);
}

void LCD_Integer(int num)
{
    char buf[10];
    int i = 0;

    if(num == 0)
    {
        LCD_Data('0');
        return;
    }

    while(num)
    {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    }

    while(i)
        LCD_Data(buf[--i]);
}

void Write_float_LCD(float num, int precision)
{
    int integer = (int)num;
    int fraction;
    int i;

    LCD_Integer(integer);
    LCD_Data('.');

    fraction = (int)((num - integer) * 100);

    if(fraction < 0)
        fraction = -fraction;

    for(i=0;i<precision;i++)
    {
        if(i==0)
            LCD_Data((fraction/10)+'0');
        else
            LCD_Data((fraction%10)+'0');
    }
}

/*==================== DS18B20 ====================*/
#define D (1<<16)     // DS18B20 on P0.16

unsigned char ResetDS18b20(void)
{
    unsigned char presence;

    IODIR0 |= D;
    IOSET0 |= D;

    DelayUs(1);

    IOCLR0 |= D;
    DelayUs(480);

    IOSET0 |= D;

    IODIR0 &= ~D;

    DelayUs(60);

    presence = (IOPIN0 & D);

    DelayUs(420);

    return (presence ? 0 : 1);
}

void WriteBit(unsigned char bit)
{
    IODIR0 |= D;

    IOCLR0 |= D;

    if(bit)
    {
        DelayUs(5);
        IOSET0 |= D;
        DelayUs(55);
    }
    else
    {
        DelayUs(60);
        IOSET0 |= D;
    }
}

unsigned char ReadBit(void)
{
    unsigned char bit;

    IODIR0 |= D;
    IOCLR0 |= D;

    DelayUs(2);

    IODIR0 &= ~D;

    DelayUs(8);

    bit = (IOPIN0 & D) ? 1 : 0;

    DelayUs(50);

    return bit;
}

void WriteByte(unsigned char data)
{
    unsigned char i;

    for(i=0;i<8;i++)
    {
        WriteBit(data & 0x01);
        data >>= 1;
    }
}

unsigned char ReadByte(void)
{
    unsigned char i, data = 0;

    for(i=0;i<8;i++)
    {
        if(ReadBit())
            data |= (1<<i);
    }

    return data;
}

int ReadTemp(void)
{
    unsigned char temp_lsb, temp_msb;
    int temp;

    ResetDS18b20();

    WriteByte(0xCC);     // Skip ROM
    WriteByte(0x44);     // Convert Temperature

    delay_ms(750);

    ResetDS18b20();

    WriteByte(0xCC);     // Skip ROM
    WriteByte(0xBE);     // Read Scratchpad

    temp_lsb = ReadByte();
    temp_msb = ReadByte();

    temp = (temp_msb << 8) | temp_lsb;

    return temp;
}

/*==================== MAIN ====================*/
int main()
{
    float temperature;

    LCD_Init();

    LCD_Command(0x80);
    LCD_String("DS18B20 TEMP");

    delay_ms(1000);

    while(1)
    {
        temperature = ReadTemp() / 16.0;

        LCD_Command(0xC0);
        LCD_String("Temp = ");

        Write_float_LCD(temperature, 2);

        LCD_String(" C   ");

        delay_ms(1000);
    }
}