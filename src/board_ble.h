#ifndef BOARD_BLE_H
#define BOARD_BLE_H

#define ENABLE_RDYN_INT()  GPIOIntEnable(BLE_RDYN_PIN, BLE_RDYN_PIN_INT);
#define DISABLE_RDYN_INT() GPIOIntDisable(BLE_RDYN_PIN, BLE_RDYN_PIN_INT);

#define SET_REQN_LOW()  GPIOPinWrite(BLE_REQN_PORTBASE, BLE_REQN_PIN, 0);
#define SET_REQN_HIGH() GPIOPinWrite(BLE_REQN_PORTBASE, BLE_REQN_PIN, BLE_REQN_PIN);

void board_ble_reset_nrf8001(void);
void board_ble_configure_pins(void);

#endif //BOARD_BLE_H