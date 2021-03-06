#ifndef BOARDCONFIG_H
#define BOARDCONFIG_H

// Peripherial configuration goes here
#define configUSE_LED						1
#define configUSE_CONSOLE_UART				1
#define configUSE_SPI						1

// Define board LEDs here
#define LED_1_PINPERIPHERIAL				SYSCTL_PERIPH_GPIOF
#define LED_2_PINPERIPHERIAL				SYSCTL_PERIPH_GPIOF
#define LED_3_PINPERIPHERIAL				SYSCTL_PERIPH_GPIOF
#define LED_ALL_PINPERIPHERIAL				SYSCTL_PERIPH_GPIOF
#define LED_1_PORTBASE						GPIO_PORTF_BASE
#define LED_2_PORTBASE						GPIO_PORTF_BASE
#define LED_3_PORTBASE						GPIO_PORTF_BASE
#define LED_ALL_PORTBASE					GPIO_PORTF_BASE
#define LED_RED_PORTBASE					LED_1_PORTBASE
#define LED_BLUE_PORTBASE					LED_2_PORTBASE
#define LED_GREEN_PORTBASE					LED_3_PORTBASE
#define LED_1_PIN 							GPIO_PIN_1
#define LED_2_PIN 							GPIO_PIN_2
#define LED_3_PIN 							GPIO_PIN_3
#define LED_RED								LED_1_PIN
#define LED_BLUE 							LED_2_PIN
#define LED_GREEN 							LED_3_PIN

// Define console UART here
#define CONSOLEUARTPERIPHERIAL				SYSCTL_PERIPH_UART0
#define CONSOLEUARTPINPERIPHERIAL			SYSCTL_PERIPH_GPIOA
#define CONSOLEUARTPERIPHERIALBASE			UART0_BASE
#define CONSOLEUARTPINPERIPHERIALBASE		GPIO_PORTA_BASE
#define CONSOLEUARTRXPINTYPE				GPIO_PA0_U0RX
#define CONSOLEUARTTXPINTYPE				GPIO_PA1_U0TX
#define CONSOLEUARTRXPIN					GPIO_PIN_0
#define CONSOLEUARTTXPIN 					GPIO_PIN_1

// Define SPI here
#define SPIPERIPHERIAL						SYSCTL_PERIPH_SSI2
#define SPIPINPERIPHERIAL					SYSCTL_PERIPH_GPIOB
#define SPIPERIPHERIALBASE					SSI2_BASE
#define SPIPINPERIPHERIALBASE				GPIO_PORTB_BASE
#define SPISCLKPINTYPE						GPIO_PB4_SSI2CLK
#define SPISFSSPINTYPE						GPIO_PB5_SSI2FSS
#define SPISOMIPINTYPE						GPIO_PB6_SSI2RX
#define SPISIMOPINTYPE						GPIO_PB7_SSI2TX
#define SPISCLKPIN							GPIO_PIN_4
#define SPISFSSPIN							GPIO_PIN_5
#define SPISOMIPIN							GPIO_PIN_6
#define SPISIMOPIN							GPIO_PIN_7

// Define SPI chip select (CS) pins here
#define SPICSPINPERIPHERIAL					SYSCTL_PERIPH_GPIOE
#define SPICSPINPERIPHERIALBASE				GPIO_PORTE_BASE
#define SPICSPIN 							GPIO_PIN_0	

// Define BLE chip pins here
#define BLE_REQN_PINPERIPHERIAL				SYSCTL_PERIPH_GPIOE // REQN = CS pin @ mikroE BLE p click module!
#define BLE_REQN_PORTBASE					GPIO_PORTE_BASE // REQN = CS pin @ mikroE BLE p click module!
#define BLE_REQN_PIN 						GPIO_PIN_0 // REQN = CS pin @ mikroE BLE p click module!

#define BLE_RST_PINPERIPHERIAL				SYSCTL_PERIPH_GPIOA
#define BLE_RST_PORTBASE					GPIO_PORTA_BASE
#define BLE_RST_PIN 						GPIO_PIN_5

#define BLE_RDYN_PINPERIPHERIAL				SYSCTL_PERIPH_GPIOE
#define BLE_RDYN_PORTBASE					GPIO_PORTE_BASE
#define BLE_RDYN_PIN 						GPIO_PIN_4
#define BLE_RDYN_PORT_INT					INT_GPIOE
#define BLE_RDYN_PIN_INT					GPIO_INT_PIN_4




#endif // BOARDCONFIG_H
