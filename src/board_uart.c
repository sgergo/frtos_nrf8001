#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#include "boardconfig.h"



static void board_init_console_uart(void) {
    // Enable the GPIO Peripheral used by the UART.
    ROM_SysCtlPeripheralEnable(CONSOLEUARTPINPERIPHERIAL);

    // Enable UART0
    ROM_SysCtlPeripheralEnable(CONSOLEUARTPERIPHERIAL);

    // Configure GPIO Pins for UART mode.
    ROM_GPIOPinConfigure(CONSOLEUARTRXPINTYPE);
    ROM_GPIOPinConfigure(CONSOLEUARTTXPINTYPE);
    ROM_GPIOPinTypeUART(CONSOLEUARTPINPERIPHERIALBASE, CONSOLEUARTRXPIN | CONSOLEUARTTXPIN);

    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(CONSOLEUARTPERIPHERIALBASE, UART_CLOCK_PIOSC);

    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 115200, 16000000);
}

void board_configure_uart(void) {
    board_init_console_uart();
}