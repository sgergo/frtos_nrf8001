#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"

#include "boardconfig.h"

static void board_ble_rdyn_handler(void) {

	GPIOIntClear(BLE_RDYN_PORTBASE, BLE_RDYN_PIN_INT);
	//SysCtlDelay(10);

	// Check if RDYN was triggered
	if((GPIOPinRead(BLE_RDYN_PORTBASE, BLE_RDYN_PIN) & BLE_RDYN_PIN) == 0) {
		// Set flag so it can be processed in the main loop
		rdynFlag = 1;
		m_rdy_line_handle();

	}

}


void board_ble_configure_pins(void) {

	//
	// Enable the GPIO ports
	//
	ROM_SysCtlPeripheralEnable(BLE_RST_PINPERIPHERIAL);
	ROM_SysCtlPeripheralEnable(BLE_REQN_PINPERIPHERIAL);
	ROM_SysCtlPeripheralEnable(BLE_RDYN_PINPERIPHERIAL);

	// Initialize GPIO

	//RESET signal as GPIO Output
	ROM_GPIODirModeSet(BLE_RST_PORTBASE, BLE_RST_PIN, GPIO_DIR_MODE_OUT);
	ROM_GPIOPadConfigSet(BLE_RST_PORTBASE, BLE_RST_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);

	//REQN signal as GPIO Output
	ROM_GPIODirModeSet(BLE_REQN_PORTBASE, BLE_REQN_PIN, GPIO_DIR_MODE_OUT);
	ROM_GPIOPadConfigSet(BLE_REQN_PORTBASE, BLE_REQN_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);


	//RDYN signal as GPIO Input
	ROM_GPIOIntRegister(BLE_RDYN_PORTBASE, board_ble_rdyn_handler);

	ROM_GPIODirModeSet(BLE_RDYN_PORTBASE, BLE_RDYN_PIN, GPIO_DIR_MODE_IN);
	ROM_GPIOPadConfigSet(BLE_RDYN_PORTBASE, BLE_RDYN_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);

	ROM_GPIOIntClear(BLE_RDYN_PORTBASE, BLE_RDYN_PIN);
	ROM_GPIOIntTypeSet(BLE_RDYN_PORTBASE,BLE_RDYN_PIN, GPIO_FALLING_EDGE);
	ROM_GPIOIntEnable(BLE_RDYN_PORTBASE, BLE_RDYN_PIN);

	IntEnable(BLE_RDYN_PORT_INT);
}
