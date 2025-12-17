#include <arch/kernel/boot.h>

void plat_uart_init(void)
{
   /* Clock frequency, per DDAI 0386C (AN386) on Table 4-2 of SYSCLK is 25MHz
       For 115200 baud operation, 25MHz/115200baud = 217.
   */
   *UART_REG(BAUDDIV) = 217;

   *UART_REG(CTRL) = CTRL_RX_ENABLE | CTRL_TX_ENABLE;
}
