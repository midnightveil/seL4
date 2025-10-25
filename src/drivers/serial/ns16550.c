#include <config.h>
#include <stdint.h>
#include <util.h>
#include <machine/io.h>
#include <plat/machine/devices_gen.h>

#define RBR     0   /* In:  Recieve Buffer Register */
#define THR     0   /* Out: Transmitter Holding Register */
#define LSR     5   /* In:  Line Status Register */

#define LSR_THRE   0x20    /* Transmit-hold-register empty */
#define LSR_DR     0x01    /* Receiver data ready */

#define REG_SHIFT 2
#define UART_REG(x) ((volatile uint32_t *)(UART_PADDR + (x << REG_SHIFT)))

#ifdef CONFIG_PRINTING
void uart_drv_putchar(unsigned char c)
{
    while ((*UART_REG(LSR) & LSR_THRE) == 0);

    *UART_REG(THR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned char uart_drv_getchar(void)
{
    while (!(*UART_REG(LSR) & LSR_DR));

    return (*UART_REG(RBR) & 0xff);
}
#endif /* CONFIG_DEBUG_BUILD */
