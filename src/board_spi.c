#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/ssi.h"

#include "boardconfig.h"

void board_spi_assert_cs(void) {
	ROM_GPIOPinWrite(SPICSPINPERIPHERIALBASE, SPICSPIN, 0); // Deassert 
}

void board_spi_deassert_cs(void) {
	ROM_GPIOPinWrite(SPICSPINPERIPHERIALBASE, SPICSPIN, SPICSPIN); // Deassert 
}

uint8_t board_spi_transfer_byte(uint8_t data) {
	uint32_t retval;
	ROM_SSIDataPut(SPIPERIPHERIALBASE, (uint32_t) data);

	while(ROM_SSIBusy(SPIPERIPHERIALBASE))
		;
	ROM_SSIDataGet(SPIPERIPHERIALBASE, &retval);

	return (uint8_t) retval;
}

static void board_init_spi(void) {
	uint32_t tempbuffer;

    // Enable Peripherals
    ROM_SysCtlPeripheralEnable(SPIPERIPHERIAL);
    ROM_SysCtlPeripheralEnable(SPIPINPERIPHERIAL);

    // Set the pin muxing
    ROM_GPIOPinConfigure(SPISCLKPINTYPE);
    // GPIOPinConfigure(SPISFSSPINTYPE);
    ROM_GPIOPinConfigure(SPISOMIPINTYPE);
    ROM_GPIOPinConfigure(SPISIMOPINTYPE);
    ROM_GPIOPinTypeSSI(SPIPINPERIPHERIALBASE, SPISCLKPIN | SPISOMIPIN | SPISIMOPIN);

    ROM_SSIConfigSetExpClk(SPIPERIPHERIALBASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_3, SSI_MODE_MASTER, 10000, 16);
    ROM_SSIEnable(SPIPERIPHERIALBASE);

    // Read residual
    while(ROM_SSIDataGetNonBlocking(SPIPERIPHERIALBASE, &tempbuffer))
    	;
}

static void board_configure_cs(void) {
    ROM_SysCtlPeripheralEnable(SPICSPINPERIPHERIAL);
    ROM_GPIOPinTypeGPIOOutput(SPICSPINPERIPHERIALBASE, SPICSPIN);
    ROM_GPIOPinWrite(SPICSPINPERIPHERIALBASE, SPICSPIN, SPICSPIN); // Deassert 
}

void board_configure_spi(void) {
	board_init_spi();
	board_configure_cs();
}
