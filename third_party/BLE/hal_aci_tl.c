/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT. 
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 * $LastChangedRevision: 4808 $
 */ 

/** @file
@brief Implementation of the ACI transport layer module
*/

#include "hal_aci_tl.h"
#include "ble_system.h"
#include <string.h>
#include "driverlib/ssi.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"


#include "boardconfig.h"
#include "board_spi.h"
#include "board_ble.h"

typedef struct {
 hal_aci_data_t           aci_data[ACI_QUEUE_SIZE];
 uint8_t                  head;
 uint8_t                  tail;
}  __attribute__ ((__packed__)) aci_queue_t;

static hal_aci_data_t received_data;
static uint8_t        spi_readwrite(uint8_t aci_byte); 
//static bool           m_print_aci_data(hal_aci_data_t *p_data);
static aci_queue_t    aci_tx_q;
static aci_queue_t    aci_rx_q;

uint8_t msbToLsb(uint8_t byte);

uint8_t rdynFlag = 0;
uint8_t spiByteRxFlag = 0;

uint8_t rxByte = 0;
static void m_aci_q_init(aci_queue_t *aci_q)
{
  aci_q->head = 0;
  aci_q->tail = 0;
}

static bool m_aci_q_enqueue(aci_queue_t *aci_q, hal_aci_data_t *p_data)
{
  const uint8_t next = (aci_q->tail + 1) % ACI_QUEUE_SIZE;
  const uint8_t length = p_data->buffer[0];
  
  if (next == aci_q->head)
  {
    /* full queue */
    return false;
  }
  aci_q->aci_data[aci_q->tail].status_byte = 0;
  // @comment : It would be better to copy the entire hal_aci_data_t into the buffer.
  memcpy((uint8_t *)&(aci_q->aci_data[aci_q->tail].buffer[0]), (uint8_t *)&p_data->buffer[0], length + 1);
  aci_q->tail = next;
  
  return true;
}

//@comment have a unit test for the queue, esp. the full and the empty states
static bool m_aci_q_dequeue(aci_queue_t *aci_q, hal_aci_data_t *p_data)
{
  if (aci_q->head == aci_q->tail)
  {
    /* empty queue */
    return false;
  }
  
  memcpy((uint8_t *)p_data, (uint8_t *)&(aci_q->aci_data[aci_q->head]), sizeof(hal_aci_data_t));
  aci_q->head = (aci_q->head + 1) % ACI_QUEUE_SIZE;
  
  return true;
}

static bool m_aci_q_is_empty(aci_queue_t *aci_q)
{
  return (aci_q->head == aci_q->tail);
}

static bool m_aci_q_is_full(aci_queue_t *aci_q)
{
  uint8_t next;
  bool state;
  
  //This should be done in a critical section
  //noInterrupts();
  IntMasterDisable();
  next = (aci_q->tail + 1) % ACI_QUEUE_SIZE;  
  
  if (next == aci_q->head)
  {
    state = true;
  }
  else
  {
    state = false;
  }
  
  //interrupts();
  IntMasterEnable();
  //end
  
  return state;
}

#if 0
static bool m_print_aci_data(hal_aci_data_t *p_data)
{
	// Disabled due to no print ability

  const uint8_t length = p_data->buffer[0];
  uint8_t i;
  Serial.print(length, DEC);
  Serial.print(" B:");
  for (i=0; i<=length; i++)
  {
    Serial.print(p_data->buffer[i], HEX);
    Serial.print(F(", "));
  }
  Serial.println(F(""));

}
#endif

void m_rdy_line_handle(void)
{
  hal_aci_data_t *p_aci_data;
  
  DISABLE_RDYN_INT();
  
  //__enable_interrupt();
  // Receive or transmit data
  p_aci_data = hal_aci_tl_poll_get();
  
  // Check if we received data
  if (p_aci_data->buffer[0] > 0)
  {
    if (!m_aci_q_enqueue(&aci_rx_q, p_aci_data))
    {
      /* Receive Buffer full.
         Should never happen.
         Will spin in an infinite loop when this happens
         */
       while (1);
    }
    if (m_aci_q_is_full(&aci_rx_q))
    {
      /* Disable RDY line interrupt.
         Will latch any pending RDY lines, so when enabled it again this
         routine should be taken again */
      //EIMSK &= ~(0x2);
      DISABLE_RDYN_INT();
    }    
  }
  ENABLE_RDYN_INT();
}

bool hal_aci_tl_event_get(hal_aci_data_t *p_aci_data)
{
  bool was_full = m_aci_q_is_full(&aci_rx_q);
  
  if (m_aci_q_dequeue(&aci_rx_q, p_aci_data))
  {
    if (was_full)
    {
      /* Enable RDY line interrupt again */
    	ENABLE_RDYN_INT();
    	//EIMSK |= (0x2);
    }
    return true;
  }
  else
  {
    return false;
  }
}

void hal_aci_tl_init()
{
	received_data.buffer[0] = 0;

	// configureNRF8001Interface();
  board_ble_configure_pins();

	/* initialize aci cmd queue */
	m_aci_q_init(&aci_tx_q);
	m_aci_q_init(&aci_rx_q);
}

bool hal_aci_tl_send(hal_aci_data_t *p_aci_cmd)
{
  const uint8_t length = p_aci_cmd->buffer[0];
  bool ret_val = false;

  if (p_aci_cmd->buffer[0] > HAL_ACI_MAX_LENGTH)
  {
  	//When the length of the ACI Message is greater than the maximum
  	//length of the ACI buffers, we will spin in a while loop
  	while(1);
  }
  
  if (length > HAL_ACI_MAX_LENGTH)
  {
    return false;
  }
  else
  {
    if (m_aci_q_enqueue(&aci_tx_q, p_aci_cmd))
    {
      ret_val = true;
    }
  }

  SET_REQN_LOW();

  return ret_val;
}

#if 0 //Get the ACI event by polling the ready line - Not used in the BLE-minimal
void hal_aci_tl_poll_rdy_line(void)
{
  uint8_t byte_cnt;
  uint8_t byte_sent_cnt;
  uint8_t max_bytes;
  hal_aci_data_t data_to_send;
  bool is_transmit_finished = false;
  
  if (1 == digitalRead(HAL_IO_RADIO_RDY))
  {
    return;
  }
    
  hal_pltf_enable_spi();  
  

  digitalWrite(HAL_IO_RADIO_REQN, 0);
  
  // Receive from queue
  if (m_aci_q_dequeue(&aci_tx_q, &data_to_send) == false)
  {
    /* queue was empty, nothing to send */
    data_to_send.status_byte = 0;
    data_to_send.buffer[0] = 0;
  }
  
  // Send length, receive header
  byte_sent_cnt = 0;
  received_data.status_byte = spi_readwrite(data_to_send.buffer[byte_sent_cnt++]);
  // Send first byte, receive length from slave
  received_data.buffer[0] = spi_readwrite(data_to_send.buffer[byte_sent_cnt++]);
  if (0 == data_to_send.buffer[0])
  {
    max_bytes = received_data.buffer[0];
  }
  else
  {
    // Set the maximum to the biggest size. One command byte is already sent
    max_bytes = (received_data.buffer[0] > (data_to_send.buffer[0] - 1)) 
      ? received_data.buffer[0] : (data_to_send.buffer[0] - 1);
  }

  if (max_bytes > HAL_ACI_MAX_LENGTH)
  {
    max_bytes = HAL_ACI_MAX_LENGTH;
  }

  // Transmit/receive the rest of the packet 
  for (byte_cnt = 0; byte_cnt < max_bytes; byte_cnt++)
  {
    received_data.buffer[byte_cnt+1] =  spi_readwrite(data_to_send.buffer[byte_sent_cnt++]);
  }
  
  digitalWrite(HAL_IO_RADIO_REQN, 1);

  hal_pltf_disable_spi();  

//   if (0 == received_data.status_byte)
//   {
//     //The debug/status byte should be 0 for a received event otherwise
//     //we will spin in this while loop
//     Serial.print(F("Error: Expected 0 as ACI Debug/Status byte in ACI Event got"));
//     Serial.println(DEC, received_data.status_byte);
//     Serial.println(F("Spinning in a while loop"));
//   	while(1);
//   }
  
  /* valid Rx available or transmit finished*/
  hal_aci_tl_msg_rcv_hook(&received_data);
}
#endif

hal_aci_data_t * hal_aci_tl_poll_get(void)
{
  uint8_t byte_cnt;
  uint8_t byte_sent_cnt;
  uint8_t max_bytes;
  hal_aci_data_t data_to_send;

  memset(data_to_send.buffer,0,sizeof(data_to_send.buffer));
  memset(received_data.buffer,0,sizeof(received_data.buffer));
  //bool is_transmit_finished = false; // Unreferenced variable

  // REQN already set
  //hal_pltf_enable_spi();
  //pinMode(5, OUTPUT);
  //digitalWrite(HAL_IO_RADIO_REQN, 0);
  
	// Set request pin low to indicate to nRF8001 we want to send data
	SET_REQN_LOW();

	// Wait for RDYN to go low, if it's not already low
	while((GPIOPinRead(BLE_RDYN_PORTBASE, BLE_RDYN_PIN) & BLE_RDYN_PIN) == 1)
    ;


  // Receive from queue
  if (m_aci_q_dequeue(&aci_tx_q, &data_to_send) == false)
  {
    /* queue was empty, nothing to send */
    data_to_send.status_byte = 0;
    data_to_send.buffer[0] = 0;
  }
  
	//Change this if your mcu has DMA for the master SPI
	//When using DMA always clock out the max length of the message being sent and
	// event being received or HAL_ACI_MAX_LENGTH
  
  // Send length, receive header
  byte_sent_cnt = 0;
  received_data.status_byte = spi_readwrite(data_to_send.buffer[byte_sent_cnt++]);
  // Send first byte, receive length from slave
  received_data.buffer[0] = spi_readwrite(data_to_send.buffer[byte_sent_cnt++]);
  if (0 == data_to_send.buffer[0])
  {
    max_bytes = received_data.buffer[0];
  }
  else
  {
    // Set the maximum to the biggest size. One command byte is already sent
    max_bytes = (received_data.buffer[0] > (data_to_send.buffer[0] - 1)) 
      ? received_data.buffer[0] : (data_to_send.buffer[0] - 1);
  }

  if (max_bytes > HAL_ACI_MAX_LENGTH)
  {
    max_bytes = HAL_ACI_MAX_LENGTH;
  }

  // Transmit/receive the rest of the packet 
  for (byte_cnt = 0; byte_cnt < max_bytes; byte_cnt++)
  {
    received_data.buffer[byte_cnt+1] =  spi_readwrite(data_to_send.buffer[byte_sent_cnt++]);
  }

  // Deassert REQN to indicate end of transmission
  SET_REQN_HIGH();
  //digitalWrite(HAL_IO_RADIO_REQN, 1);
  //hal_pltf_disable_spi();
  //RDYN should follow the REQN line in approx 100ns
  
  SysCtlDelay(100); // Delay to allow REQN to stay high for the required amount of time


  //If there are more ACI commands in the queue, lower the REQN line immediately
  if (false == m_aci_q_is_empty(&aci_tx_q))
  {
      //digitalWrite(HAL_IO_RADIO_REQN, 0);
	  // Set request pin low to indicate to nRF8001 we want to send data
		SET_REQN_LOW();

		// Wait for RDYN to be called
		while((GPIOPinRead(BLE_RDYN_PORTBASE, BLE_RDYN_PIN) & BLE_RDYN_PIN) == 1)
      ;
  }
  
  //sleep_enable();
  //attachInterrupt(1, m_rdy_line_handle, LOW);

  /*
  // Enable interrupts and enter LPM0 so we wait for the RDYN to be
  // set low by the nRF8001, indicating its ready to receive data
  	if(nRF8001_RDYN_PINREG & nRF8001_RDYN_PIN)
  	{
  		do
  		{
  			// Wait for nRF8001 to indicate it is ready by waiting for RDYN
  			_BIS_SR(LPM0_bits + GIE); // Enter LPM0 w/interrupt
  			_nop();
  		}while(rdynFlag == 0);
  	}
	*/

  
  /* valid Rx available or transmit finished*/
  return (&received_data);
}

// Receive a byte from the nRF8001 by sending
// an ACI_BYTE.
// static uint8_t spi_readwrite(const uint8_t aci_byte)
// {
// 	spiByteRxFlag = 0;



// 	// Send byte and wait for byte back

// 	/*

// 	// Clear RX IFG
// 	SPI_IFG &= ~(SPI_RX_IFG);

// 	// Wait for SPI TX buffer to be ready
// 	while(!(SPI_IFG & SPI_TX_IFG));
// 	*/

// 	SSIDataPut(SSI2_BASE,msbToLsb(aci_byte));

// 	// Send byte over SPI
// 	//SPI_TX_BUF = aci_byte;

// 	// Wait until a byte has been received
// 	SSIDataGet(SSI2_BASE,&rxByte);

// 	volatile uint8_t rxB = rxByte;
// 	volatile uint8_t rxLSB = msbToLsb(rxB);

// 	return rxLSB;
// }

static uint8_t spi_readwrite(const uint8_t aci_byte)
{
  spiByteRxFlag = 0;
  volatile uint8_t rxB;



  // Send byte and wait for byte back
  // CS is managed before/after this function call

  rxByte = board_spi_transfer_byte(aci_byte);
  rxB = rxByte;
  
  // return msbToLsb(rxB);
  return rxB;
}

// USCI A0/B0 RX interrupt vector
/*#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCIA0RX_ISR(void)
{
	if(SPI_IFG & SPI_RX_IFG)
	{
		// Byte was received, so set flag and clear interrupt
		// flag
		SPI_IFG &= ~(SPI_RX_IFG);
		spiByteRxFlag = 1;

		rxByte = UCB0RXBUF;

		// Exit from sleep mode upon ISR exit so data can be
		// processed
		_BIC_SR_IRQ( LPM0_bits );
	}
}*/

uint8_t msbToLsb(uint8_t byte)
{
	int i = 0;
	uint8_t lsbByte = 0;
	for(i = 0; i < 8; i++)
	{
		if(byte & (1 << (7-i)))
			lsbByte |= (0x01 << i);
	}

	return lsbByte;
}
