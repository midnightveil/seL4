#include <config.h>
#include <stdint.h>
#include <util.h>
#include <machine/io.h>
#include <plat/machine.h>

/**
 * Documentation as per "Arm Cortex-M System Design Kit" DDI 0479D
 * https://documentation-service.arm.com/static/5e8f224c7100066a414f7810
 * Section 4.3 "APB UART" and 4.3.1 "Programmers model"
 **/

#define DATA  0x000   /* RW: Receive/Transmit Data */
#define STATE 0x004   /* RW: State (buffers) */
#define CTRL  0x008   /* RW: Control */

#define INTSTATUS   0x00C   /* R: Interrupt status */
#define INTCLEAR    0x00C   /* W: Interrupt clear */

#define BAUDDIV    0x010   /* RW: Baud rate divisor */

/* STATE register */
#define STATE_RX_OVERRUN    BIT(3) /* write 1 to clear */
#define STATE_TX_OVERRUN    BIT(2) /* write 1 to clear */
#define STATE_RX_FULL       BIT(1) /* read-only */
#define STATE_TX_FULL       BIT(0) /* read-only */

/* CTRL register */
#define CTRL_HIGH_SPEED_TX          BIT(6) /* high speed test mode */
#define CTRL_RX_OVERRUN_ENABLE      BIT(5)
#define CTRL_TX_OVERRUN_ENABLE      BIT(4)
#define CTRL_RX_INTERRUPT_ENABLE    BIT(3)
#define CTRL_TX_INTERRUPT_ENABLE    BIT(2)
#define CTRL_RX_ENABLE              BIT(1)
#define CTRL_TX_ENABLE              BIT(0)

/* INTSTATUS/INTCLEAR */
#define INT_RX_OVERRUN  BIT(3) /* write 1 to clear */
#define INT_TX_OVERRUN  BIT(2) /* write 1 to clear */
#define INT_RX          BIT(1) /* write 1 to clear */
#define INT_TX          BIT(0) /* write 1 to clear */

#define UART_REG(x) ((volatile uint32_t *)(UART_PADDR + x))

#ifdef CONFIG_PRINTING
void uart_init(void);
void uart_init(void)
{
    /* Clock frequency, per DDAI 0386C (AN386) on Table 4-2 of SYSCLK is 25MHz
       For 115200 baud operation, 25MHz/115200baud = 217.
    */
    *UART_REG(BAUDDIV) = 217;

    *UART_REG(CTRL) = CTRL_RX_ENABLE | CTRL_TX_ENABLE;
}

void uart_drv_putchar(unsigned char c)
{
    while ((*UART_REG(STATE) & STATE_TX_FULL));

    *UART_REG(DATA) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned char uart_drv_getchar(void)
{
    while (!(*UART_REG(STATE) & STATE_RX_FULL));

    return (*UART_REG(DATA) & 0xff);
}
#endif /* CONFIG_DEBUG_BUILD */
