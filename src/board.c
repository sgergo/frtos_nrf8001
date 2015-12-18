#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"

#include "boardconfig.h"
#include "utils/uartstdio.h"
#include "board_spi.h"
#include "board_uart.h"


static void board_configure_led(void) {
	ROM_SysCtlPeripheralEnable(LED_ALL_PINPERIPHERIAL);
    ROM_GPIOPinTypeGPIOOutput(LED_ALL_PORTBASE, LED_RED|LED_BLUE|LED_GREEN);
    ROM_GPIOPinWrite(LED_ALL_PORTBASE, LED_RED|LED_GREEN|LED_BLUE, 0); // Switch off all LEDs
}

static void board_configure_systemclock(void) {
    // Set the clocking to run at 50 MHz from the PLL.
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);
}

void board_init(void) {

    board_configure_systemclock();

#if (configUSE_LED == 1)
	board_configure_led();
#endif // configUSE_LED

#if (configUSE_CONSOLE_UART == 1)
	board_configure_uart();
#endif // configUSE_CONSOLE_UART

#if (configUSE_SPI == 1)
    board_configure_spi();
#endif // configUSE_SPI
}