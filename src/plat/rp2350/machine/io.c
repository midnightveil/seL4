#include <arch/kernel/boot.h>
#include <config.h>

#ifdef CONFIG_PRINTING

#define PUT32(address, value) (*((volatile unsigned int *)(address))) = value
#define GET32(address) (*(volatile unsigned int *)(address))


/* Define Atomic Register Access
   See section 2.1.3 "Atomic Register Access" on RP2350 datasheet */
#define WRITE_NORMAL (0x0000)   // normal read write access
#define WRITE_XOR    (0x1000)   // atomic XOR on write
#define WRITE_SET    (0x2000)   // atomic bitmask set on write
#define WRITE_CLR    (0x3000)   // atomic bitmask clear on write

void plat_uart_init(void) {
    // Setup XOC clock to drive the GPIO (Pico2 board as a ABM8-272-T3 crystal that oscillates at 12MHz)
    PUT32((0x40048000 + 0),      0x00000aa0);               //  XOC range 1-15MHz (Crystal Oschillator)
    PUT32((0x40048000 + 0x0c),   0x000000c4);               //  Startup Delay (default = 50,000 cycles aprox.)
    PUT32((0x40048000 + 0x2000), 0x00FAB000);               //  Enable XOC
    while (!(GET32(0x40048000 + 4) & ( 1 << 31 )));         //  Wait for XOC stable

    // Configure source clock for components (see datasheer RP2350 Chapter 8. "Clocks")
    PUT32((0x40010000 + 0x3C), 0 );                         //  CLK SYS CTRL = XOC (for processor, bus frabric & memories)
    PUT32((0x40010000 + 0x48), ((1 << 11) | ( 4 << 5)));    //  CLK_PERI_CTRL = XOC (for perifery UART and SPI) + Enable

    // De-asserts the reset of UART0
    PUT32((0x40020000 + WRITE_SET + 0x0), (1 << 26));       // Set UART0 to reset
    asm("nop");
    asm("nop");
    PUT32((0x40020000 + WRITE_CLR + 0x0), (1 << 26));       // De-assert the reset from UART0
    while (!(GET32(0x40020000 + 0x08) & (1 << 26)));        // Wait for UART0 to be ready

    // Configure GPIO25 to use function 5 (SIO) to controll the GPIO by software
    PUT32((0x40028000 + 0xcc), 5);                          // IO GPIO25 uses SIO
    PUT32((0x40028000 + 0x04), 2);                          // IO GPIO0 uses UART TX
    PUT32((0x40028000 + 0x0c), 2);                          // IO GPIO1 uses UART RX

    // Enable GPIO out in SIO register
    PUT32((0xd0000000 + WRITE_SET + 0x038), (1 << 25));     // SIO OE (output enable) for Pin25

    // Configure the pad control (new on RP2350)
    PUT32((0x40038000 + WRITE_CLR + 0x68), (1 << 8));       // Remove GPIO25 pad isolation
    PUT32((0x40038000 + WRITE_CLR + 0x04), (1 << 8));       // Remove UART0TX pad isolation
    PUT32((0x40038000 + WRITE_CLR + 0x08), (1 << 8));       // Remove UART0RX pad isolation
    PUT32((0x40038000 + WRITE_SET + 0x08), (1 << 6));       // Enable UART0RX pad for input

    // Configure UART0
    //   Baud: For a baud rate of 115200 with UARTCLK = 12MHz then:
    //   Baud Rate Divisor = 12000000/(16 * 115200) ~= 6.5104
    PUT32((0x40070000 + 0x24), 6);                                         // UARTIBRD_H: Integer part of the baudrate divisor
    PUT32((0x40070000 + 0x28), 5104);                                      // UARTFBRD_L: Decimal part of the baudrate divisor
    PUT32((0x40070000 + 0x2c), (( 0x3 << 5 ) | ( 1 << 4 )));               // UARTLCR_H: Word lenght = 8, FIFO RX/TX enabled
    PUT32((0x40070000 + 0x30), ((   1 << 9 ) | ( 1 << 8 ) | ( 1 << 0 )));  // UARTCR: UART Enabled, Tx enabled, Rx enabled
}

#endif
