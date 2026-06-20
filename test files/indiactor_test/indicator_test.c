#include <lpc21xx.h>

/*==================== DELAY ====================*/

void delay_ms(unsigned int ms)
{
    unsigned int i, j;

    for(i = 0; i < ms; i++)
        for(j = 0; j < 12000; j++);
}

/*==================== DEFINES ====================*/

#define LED_POS 4   /* P0.4 to P0.11 */

/*================ LEFT INDICATOR ====================*/

unsigned char L[8] =
{
    0x7F,
    0xBF,
    0xDF,
    0xEF,
    0xF7,
    0xFB,
    0xFD,
    0xFE
};

/*================ RIGHT INDICATOR ====================*/

unsigned char R[8] =
{
    0xFE,
    0xFD,
    0xFB,
    0xF7,
    0xEF,
    0xDF,
    0xBF,
    0x7F
};

/*================ FLAGS ====================*/

volatile unsigned char left_flag = 0;
volatile unsigned char right_flag = 0;

/*================ WRITE LED ====================*/

void write_led(unsigned char data)
{
    /* Clear P0.4 to P0.11 */
    IOCLR0 = (0xFF << LED_POS);

    /* Write pattern */
    IOSET0 = ((unsigned int)data << LED_POS);
}

/*================ EINT1 ISR ====================*/
/* LEFT SWITCH -> P0.14 */

void eint1_isr(void)__irq
{
    left_flag = !left_flag;

    if(left_flag)
        right_flag = 0;

    /* Clear EINT1 interrupt */
    EXTINT = (1 << 1);

    VICVectAddr = 0;
}

/*================ EINT2 ISR ====================*/
/* RIGHT SWITCH -> P0.15 */

void eint2_isr(void)__irq
{
    right_flag = !right_flag;

    if(right_flag)
        left_flag = 0;

    /* Clear EINT2 interrupt */
    EXTINT = (1 << 2);

    VICVectAddr = 0;
}

/*================ ENABLE EINT1 ====================*/

void enable_eint1(void)
{
    /* P0.14 = EINT1 */
    PINSEL0 |= (2 << 28);

    /* IRQ mode */
    VICIntSelect &= ~(1 << 15);

    /* Enable interrupt */
    VICIntEnable |= (1 << 15);

    /* Slot 0 */
    VICVectCntl0 = (1 << 5) | 15;

    /* ISR Address */
    VICVectAddr0 = (unsigned int)eint1_isr;

    /* Edge Triggered */
    EXTMODE |= (1 << 1);

    /* Falling Edge */
    EXTPOLAR &= ~(1 << 1);
}

/*================ ENABLE EINT2 ====================*/

void enable_eint2(void)
{
    /* P0.15 = EINT2 */
    PINSEL0 |= (2 << 30);

    /* IRQ mode */
    VICIntSelect &= ~(1 << 16);

    /* Enable interrupt */
    VICIntEnable |= (1 << 16);

    /* Slot 1 */
    VICVectCntl1 = (1 << 5) | 16;

    /* ISR Address */
    VICVectAddr1 = (unsigned int)eint2_isr;

    /* Edge Triggered */
    EXTMODE |= (1 << 2);

    /* Falling Edge */
    EXTPOLAR &= ~(1 << 2);
}

/*==================== MAIN ====================*/

int main()
{
    unsigned char i;

    /* P0.4 to P0.11 Output */
    IODIR0 |= (0xFF << LED_POS);

    /* OFF all LEDs */
    IOSET0 = (0xFF << LED_POS);

    /* Enable interrupts */
    enable_eint1();
    enable_eint2();

    while(1)
    {
        /*=========== LEFT INDICATOR ===========*/

        if(left_flag)
        {
            for(i = 0; i < 8; i++)
            {
                write_led(L[i]);

                delay_ms(150);

                /* Immediate switch */
                if(right_flag || !left_flag)
                    break;
            }
        }

        /*=========== RIGHT INDICATOR ===========*/

        else if(right_flag)
        {
            for(i = 0; i < 8; i++)
            {
                write_led(R[i]);

                delay_ms(150);

                /* Immediate switch */
                if(left_flag || !right_flag)
                    break;
            }
        }

        /*=========== BOTH OFF ===========*/

        else
        {
            /* OFF LEDs (Active LOW) */
            IOSET0 = (0xFF << LED_POS);
        }
    }
}