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

#include "BLE/hal_aci_tl.h"
#include "BLE/aci.h"
#include "BLE/aci_evts.h"
#include "BLE/aci_cmds.h"
#include "BLE/lib_aci.h"

xSemaphoreHandle g_pUARTSemaphore;

static struct aci_state_t aci_state;
static hal_aci_evt_t aci_data;
static hal_aci_data_t aci_cmd;

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
    // g_pUARTSemaphore = xSemaphoreCreateMutex();

    // Initializing tasks goes here

    // Start the scheduler.  This should not return.
    // vTaskStartScheduler();

    // In case the scheduler returns for some reason, print an error and loop
    // forever.
    // while(1)
       //  ;
    hal_aci_tl_init();
    board_ble_reset_nrf8001();
    IntMasterEnable();

    while(1)
    {
#if 0
        // We enter the if statement only when there is a ACI event available to be processed
        if (lib_aci_event_get(&aci_state, &aci_data))
        {

            aci_evt_t * aci_evt;
            board_red_led_on();
            aci_evt = &aci_data.evt;
            switch(aci_evt->evt_opcode)
            {
                /**
                As soon as you reset the nRF8001 you will get an ACI Device Started Event
                */

                UARTprintf("aci_evt->evt_opcode: %02x\n", aci_evt->evt_opcode);
                case ACI_EVT_DEVICE_STARTED:
                {

                  aci_state.data_credit_available = aci_evt->params.device_started.credit_available;
                  switch(aci_evt->params.device_started.device_mode)
                  {
                    case ACI_DEVICE_SETUP:

                      /**
                      Device is in the setup mode
                      */
                      UARTprintf("Evt Device Started: Setup\n");
                      aci_cmd.buffer[0] = 2;    //Length of ACI command
                      aci_cmd.buffer[1] = 0x01; //Command - Test
                      aci_cmd.buffer[2] = ACI_TEST_MODE_DTM_UART; //Command parameter
                      hal_aci_tl_send(&aci_cmd);
                      break;

                    case ACI_DEVICE_TEST:
                        UARTprintf("Evt Device Started: Test\n");

                      break;
                  }
                }
                break; //ACI Device Started Event
              case ACI_EVT_CMD_RSP:
                //If an ACI command response event comes with an error -> stop
                if (ACI_STATUS_SUCCESS != aci_evt->params.cmd_rsp.cmd_status)
                {
                  //ACI ReadDynamicData and ACI WriteDynamicData will have status codes of
                  //TRANSACTION_CONTINUE and TRANSACTION_COMPLETE
                  //all other ACI commands will have status code of ACI_STATUS_SCUCCESS for a successful command
                  //Serial.print(F("ACI Command "));
                  //Serial.println(aci_evt->params.cmd_rsp.cmd_opcode, HEX);
                  //Serial.println(F("Evt Cmd respone: Error. In an while(1); loop"));
                  while (1);
                }
                break;
            }
        }
        else
        {
        //Serial.println(F("No ACI Events available"));
        // No event in the ACI Event queue.
        // Arduino can go to sleep
        // Wakeup from sleep from the RDYN line
            board_green_led_on();
        }
#endif
    }
}