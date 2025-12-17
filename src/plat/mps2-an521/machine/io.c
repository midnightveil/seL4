#include <arch/kernel/boot.h>

void plat_uart_init(void)
{
   /* Clock frequency, per DDAI 0521C (AN521) on Table 5-4 of SYSCLK is 20MHz
       For 115200 baud operation, 20MHz/115200baud = 174.
   */
   *UART_REG(BAUDDIV) = 174;

   *UART_REG(CTRL) = CTRL_RX_ENABLE | CTRL_TX_ENABLE;
}
