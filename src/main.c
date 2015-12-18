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

#include "board.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

xSemaphoreHandle g_pUARTSemaphore;

void
vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName) {
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    while(1)
	    ;
}

int main(void) {

    // Initialize the board specific stuff here
    board_init();

    // Print demo introduction.
    UARTprintf("\nWelcome to the EK-TM4C123GXL FreeRTOS Project Zero!\n");

    // Create a mutex to guard the UART (not used here)
    g_pUARTSemaphore = xSemaphoreCreateMutex();

    // Initializing tasks goes here

    // Start the scheduler.  This should not return.
    vTaskStartScheduler();

    // In case the scheduler returns for some reason, print an error and loop
    // forever.
    while(1)
	    ;
}