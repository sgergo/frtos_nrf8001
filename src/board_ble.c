#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

#include "boardconfig.h"
#include "board.h"

#include "BLE/hal_aci_tl.h"
#include "BLE/aci.h"
#include "BLE/aci_evts.h"
#include "BLE/aci_cmds.h"
#include "BLE/lib_aci.h"



static void board_ble_rdyn_handler(void) {

	GPIOIntClear(BLE_RDYN_PORTBASE, BLE_RDYN_PIN_INT);
	//SysCtlDelay(10);
	// Check if RDYN was triggered
	if((GPIOPinRead(BLE_RDYN_PORTBASE, BLE_RDYN_PIN) & BLE_RDYN_PIN) == 0) {

		// Set flag so it can be processed in the main loop
		// rdynFlag = 1;
		board_blue_led_on();
		m_rdy_line_handle();
	}
}

// Resets the nRF8001 device, generating a Device Started event
void board_ble_reset_nrf8001(void) {
	// Release reset

	// Set HIGH
	ROM_GPIOPinWrite(BLE_RST_PORTBASE, BLE_RST_PIN, BLE_RST_PIN);
	ROM_SysCtlDelay(10);

	// Set LOW
	ROM_GPIOPinWrite(BLE_RST_PORTBASE, BLE_RST_PIN, 0);
	ROM_SysCtlDelay(100);

	// Set HIGH
	ROM_GPIOPinWrite(BLE_RST_PORTBASE, BLE_RST_PIN, BLE_RST_PIN);
	ROM_SysCtlDelay(10);
}


void board_ble_configure_pins(void) {

	//
	// Enable the GPIO ports
	//
	ROM_SysCtlPeripheralEnable(BLE_RST_PINPERIPHERIAL);
	// ROM_SysCtlPeripheralEnable(BLE_REQN_PINPERIPHERIAL);	//REQN = CS pin @ mikroE BLE p click module!
	ROM_SysCtlPeripheralEnable(BLE_RDYN_PINPERIPHERIAL);

	// Initialize GPIO

	//RESET signal as GPIO Output
	ROM_GPIODirModeSet(BLE_RST_PORTBASE, BLE_RST_PIN, GPIO_DIR_MODE_OUT);
	ROM_GPIOPadConfigSet(BLE_RST_PORTBASE, BLE_RST_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);

	//REQN signal as GPIO Output
	//REQN = CS pin @ mikroE BLE p click module!

	// ROM_GPIODirModeSet(BLE_REQN_PORTBASE, BLE_REQN_PIN, GPIO_DIR_MODE_OUT);
	// ROM_GPIOPadConfigSet(BLE_REQN_PORTBASE, BLE_REQN_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);


	//RDYN signal as GPIO Input
	GPIOIntRegister(BLE_RDYN_PORTBASE, board_ble_rdyn_handler);

	ROM_GPIODirModeSet(BLE_RDYN_PORTBASE, BLE_RDYN_PIN, GPIO_DIR_MODE_IN);
	ROM_GPIOPadConfigSet(BLE_RDYN_PORTBASE, BLE_RDYN_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);

	GPIOIntClear(BLE_RDYN_PORTBASE, BLE_RDYN_PIN_INT);
	ROM_GPIOIntTypeSet(BLE_RDYN_PORTBASE,BLE_RDYN_PIN_INT, GPIO_FALLING_EDGE);
	GPIOIntEnable(BLE_RDYN_PORTBASE, BLE_RDYN_PIN_INT);

	IntEnable(BLE_RDYN_PORT_INT);
}
