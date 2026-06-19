/******************************************************************************
 * Function Name : delay_us
 * Description   : Generates an approximate delay in microseconds.
 *
 * Parameter:
 *   tdly - Delay time in microseconds.
 *
 * Working:
 *   - LPC2129 is assumed to run with a 12 MHz clock.
 *   - One clock cycle ˜ 1/12 MHz = 0.083 µs.
 *   - Multiplying tdly by 12 converts the required delay
 *     into the approximate number of clock cycles.
 *   - The while loop consumes CPU cycles to create the delay.
 *
 * Note:
 *   - This is a software delay and is not highly accurate.
 *   - Delay depends on compiler optimization and CPU frequency.
 ******************************************************************************/
void delay_us(unsigned int tdly)
{
    tdly *= 12;          // Convert microseconds to loop count
    while(tdly--);       // Wait until count reaches zero
}

/******************************************************************************
 * Function Name : delay_ms
 * Description   : Generates an approximate delay in milliseconds.
 *
 * Parameter:
 *   tdly - Delay time in milliseconds.
 *
 * Working:
 *   - 1 millisecond = 1000 microseconds.
 *   - At 12 MHz, approximately 12000 clock cycles occur in 1 ms.
 *   - Multiplying tdly by 12000 gives the required loop count.
 *   - The while loop creates the delay by wasting CPU cycles.
 *
 * Note:
 *   - Accuracy depends on processor clock and compiler settings.
 *   - Not suitable for precise timing applications.
 ******************************************************************************/
void delay_ms(unsigned int tdly)
{
    tdly *= 12000;       // Convert milliseconds to loop count
    while(tdly--);       // Wait until count reaches zero
}

/******************************************************************************
 * Function Name : delay_s
 * Description   : Generates an approximate delay in seconds.
 *
 * Parameter:
 *   tdly - Delay time in seconds.
 *
 * Working:
 *   - At 12 MHz, approximately 12,000,000 clock cycles occur in 1 second.
 *   - Multiplying tdly by 12,000,000 gives the required loop count.
 *   - The while loop executes until the count becomes zero.
 *
 * Note:
 *   - This method blocks the CPU during the entire delay period.
 *   - For long delays, hardware timers are recommended instead.
 *   - Large delay values may cause integer overflow.
 ******************************************************************************/
void delay_s(unsigned int tdly)
{
    tdly *= 12000000;    // Convert seconds to loop count
    while(tdly--);       // Wait until count reaches zero
}
