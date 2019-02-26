/* Filename: lora.c
 * Last Edited: 7 May 2019
 * Author: TritonCubed
 *
 * This is a small library for the RFM95/96/97/98 in LoRa mode.
 * This is an adaptatoin of the library for the SX1278 for the
 * "Shannon" payload. The module operates around 868MHz
 */

#include "lora.h"
#include "bcm2835.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t lora_init() {

  // Get current mode
  uint8_t bootmode = read_reg( REG_OP_MODE );

  // If not in LoRa mode, switch
  if( !(bootmode & LORA_MODE) ) {
    write_reg( REG_OP_MODE, FSK_SLEEP );
  }
  
  // Switch to LoRa sleep mode
  write_reg( REG_OP_MODE, LORA_SLEEP );
  
  // Set the frequency
  uint64_t freq = DEFAULT_LORA_FREQ;
  write_reg( REG_RF_FREQ_MSB, (uint8_t)(freq >> 16) );
  write_reg( REG_RF_FREQ_MID, (uint8_t)(freq >> 8) );
  write_reg( REG_RF_FREQ_LSB, (uint8_t)(freq >> 0) );
  
  // Initialize RX/TX stack
  write_reg( REG_FIFO_TX_BASE_ADDR, 0 );
  write_reg( REG_FIFO_RX_BASE_ADDR, 0 );
  
  // Set the payload size
  write_reg( REG_PAYLOAD_LEN, BUFFER_SIZE );
  write_reg( REG_MAX_PAYLOAD_LEN, BUFFER_SIZE );
  
  // Set LNA boost
  write_reg( REG_LNA, read_reg( REG_LNA ) | DEFAULT_LORA_LNA );

  // Set auto AGC
  write_reg( REG_MODEM_CONFIG3, DEFAULT_LORA_AGC );
  
  // Set SYNC word
  write_reg( REG_SYNC_WORD, DEFAULT_LORA_SYNC_WORD );
  
  // Switch to LoRa standby mode
  write_reg( REG_OP_MODE, LORA_STANDBY );
  
  //check that the module is in standby mode
  bootmode = read_reg( REG_OP_MODE );
  if( bootmode != LORA_STANDBY ) {
    fprintf( stderr, "Could not initialize LORA\r\nBoot mode is 0x%x\r\n", bootmode );
    return 0;
  }
  
  printf( "LORA on standby\r\n" );
  return 1;
}

void lora_close() {
	write_reg( REG_OP_MODE, LORA_SLEEP );
}

uint8_t read_reg( uint8_t reg ){
  char tbuf[] = { reg , 0x00 };
  char rbuf[] = { 0x00, 0x00 };
  bcm2835_spi_transfernb( tbuf, rbuf, sizeof( tbuf ) );
  
  return rbuf[1];
}

uint8_t write_reg( uint8_t reg, char data ) {
  char tbuf[] = { 0x80 | reg , data}; //flag reg MSB high to indicate write
  char rbuf[] = { 0x00, 0x00 };
  bcm2835_spi_transfernb( tbuf, rbuf, sizeof( tbuf ) );

  return rbuf[1];
}